package com.hmdp.service.impl;

import cn.hutool.core.bean.BeanUtil;
import cn.hutool.core.collection.CollUtil;
import cn.hutool.core.util.BooleanUtil;
import cn.hutool.core.util.StrUtil;
import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.hmdp.dto.Result;
import com.hmdp.dto.ScrollResult;
import com.hmdp.dto.UserDTO;
import com.hmdp.entity.Blog;
import com.hmdp.entity.Follow;
import com.hmdp.entity.User;
import com.hmdp.mapper.BlogMapper;
import com.hmdp.service.IBlogService;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.hmdp.service.IFollowService;
import com.hmdp.service.IUserService;
import com.hmdp.utils.RedisConstants;
import com.hmdp.utils.SystemConstants;
import com.hmdp.utils.UserHolder;
import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.data.redis.core.ZSetOperations;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import static com.hmdp.utils.RedisConstants.FEED_KEY;

/**
 * <p>
 *  服务实现类
 * </p>
 *
 * @author 虎哥
 * @since 2021-12-22
 */

@Service
public class BlogServiceImpl extends ServiceImpl<BlogMapper, Blog> implements IBlogService {
    @Resource
    private IUserService userService;
    @Resource
    private StringRedisTemplate stringRedisTemplate;
    @Resource
    private IFollowService followService;
    @Override
    public Result queryHotBlog(Integer current) {
        // 查询表的like字段，返回前10个点赞数降序的字段，select * from blog order by liked desc limit 1页, 10;
        Page<Blog> page = query()
                .orderByDesc("liked")
                .page(new Page<>(current, SystemConstants.MAX_PAGE_SIZE));
        // 获取当前页数据
        List<Blog> records = page.getRecords();
        // 给要展示的每一个blog添加用户名，头像和是否被当前用户点赞
        records.forEach(blog -> {
            this.queryBlogUser(blog);   // 查询发表blog的用户
            this.isBlogLiked(blog); // 查询blog是否被当前用户点赞
        });
        return Result.ok(records);
    }

//查询id获得bolg，返回即可
    @Override
    public Result queryBlogById(Long id) {
        //1.查询blog
        Blog blog = getById(id);
        if (blog==null) {
            return Result.fail("笔记不存在！");
        }
        //2.查询blog有关用户
        queryBlogUser(blog);
        //3.查询blog是否被当前用户点赞
        isBlogLiked(blog);
        return Result.ok(blog);
    }

    private void isBlogLiked(Blog blog) {
        // 1. 获取当前登录用户的 ID
        Long userId = UserHolder.getUser().getId();

        // 2. 构建 Redis 中的 key，格式为 "blog:liked:{博客ID}"
        String key = RedisConstants.BLOG_LIKED_KEY + blog.getId();

        // 3. 从 Redis 的 ZSet 中查询用户 ID 是否存在，并获取其分数（score）
        //    如果存在，说明用户点过赞，score 为点赞时间戳
        //    如果不存在，返回 null
        Double score = stringRedisTemplate.opsForZSet().score(key, userId.toString());

        // 4. 将查询结果（用户是否点赞）设置到博客对象的 isLike 属性中
        //    score != null 表示用户点过赞，转换为布尔值
        blog.setIsLike(BooleanUtil.isTrue(score != null));
    }

    //判断并修改点赞数
    @Override
    public Result likeBlog(Long id) {
        Long userId = UserHolder.getUser().getId();

        String key="blog:liked:"+id;
        Double score = stringRedisTemplate.opsForZSet().score(key, userId.toString());//ZSCORE key member
        if(score ==null){//未点赞
            //update your_table_name set liked = liked + 1 where id = ?;
            boolean isSuccess = update().setSql("liked=liked+1").eq("id", id).update();
            if (isSuccess) {//ture更改成功，更新redis中的  zadd key  member score
                stringRedisTemplate.opsForZSet().add(key,userId.toString(),System.currentTimeMillis());
            }
        }else {
            boolean isSuccess = update().setSql("liked=liked-1").eq("id", id).update();
            if (isSuccess) {
                stringRedisTemplate.opsForZSet().remove(key,userId.toString());
            }
        }
        return Result.ok();
    }

    @Override
    public Result queryBlogLikes(Long id) {
        // 查询top5的点赞用户 zrange key 0 4
        String key = RedisConstants.BLOG_LIKED_KEY + id;
        //ZRANGE rank:user:likes 0 4
        Set<String> top5 = stringRedisTemplate.opsForZSet().range(key, 0, 4);
        // 检查top5集合是否为空，如果为空则直接返回一个空列表的成功响应
        if (CollUtil.isEmpty(top5)) {
            return Result.ok(Collections.emptyList());
        }
        // 把从 Redis 拿到的top5集合（原本是字符串类型）转成Long类型的列表。
        List<Long> userIds = top5.stream().map(Long::valueOf).collect(Collectors.toList());
        String userIdsStr = StrUtil.join(",", userIds);	// 如“6，2，1”
        // 数据库查询与排序， WHERE id IN (6, 2, 1) ORDER BY FIELD(id, 6, 2, 1) FIELD函数用于根据指定的顺序对结果进行排序
        List<UserDTO> userDTOS = userService.query()
                .in("id", userIds).last("ORDER BY FIELD(id, " + userIdsStr + ")").list()
                    //再把user转为userDTO，收集到collect集合里
                .stream()
                .map(user -> BeanUtil.copyProperties(user, UserDTO.class))
                .collect(Collectors.toList());
        // 返回UserVo集合
        return Result.ok(userDTOS);
    }

    @Override
    public Result saveBlog(Blog blog) {//为什么这里错误？
        // 获取登录用户
                    UserDTO user = UserHolder.getUser();
       if(blog.getShopId()==null){
           return Result.fail("必须选择商铺");
       }
            blog.setUserId(user.getId());
            // 保存探店笔记
         try{
             boolean isSuccess = save(blog); //？？？
         }catch (Exception e){
             log.debug("保存失败{}");
         }
        // 查询笔记作者（当前登录用户）的所有粉丝（注意：之前Redis中存的是某用户关注的人，不是某用户的粉丝）select * from tb_follow where follow_user_id = ?
        List<Follow> follows = followService.lambdaQuery().eq(Follow::getFollowUserId, user.getId()).list();
        // 推送笔记id给所有粉丝
        for (Follow follow : follows) {
            // 获取粉丝id
            Long fanId = follow.getUserId();
            // 推送笔记id
            String key = FEED_KEY + fanId;
            stringRedisTemplate.opsForZSet().add(key, blog.getId().toString(), System.currentTimeMillis());
        }
        // 返回id
        return Result.ok(blog.getId());
    }

    @Override
    public Result queryBlogOfFollow(Long max, Integer offset) {
        //Long.valueOf(typedTuple.getValue())，typedTuple.getScore().longValue().这里是有序遍历找最小值：
      //参数是最大时间戳（当前时间），offset默认是0
        //返回值是有完整的blogList，minTime，offset
        //获取当前userId，查它的feed。key，max,min,offset,count
        Long userId = UserHolder.getUser().getId();
        String key= FEED_KEY+userId;
        Set<ZSetOperations.TypedTuple<String>> typedTuples = stringRedisTemplate.opsForZSet()
                .reverseRangeByScoreWithScores(key, 0, max, offset, 3);
        //空。解析到blogId，minTime，offset。完善blog，封装返回
        if(typedTuples.isEmpty()  | typedTuples==null){
            return Result.ok();
        }
        List<Long> blogIds=new ArrayList<>(typedTuples.size());//8 6 5 5
        Long minTime = 0L;
        int os=1;
        for (ZSetOperations.TypedTuple<String> typedTuple : typedTuples) {
             blogIds.add(Long.valueOf(typedTuple.getValue()));
             if(minTime==typedTuple.getScore().longValue()){
                 os++;
             }else{
                 minTime=typedTuple.getScore().longValue();
                 os= 1;
             }
        }
        //ids->query->完善
        String idStr = StrUtil.join(",", blogIds);
        List<Blog> blogs = this.list(new LambdaQueryWrapper<Blog>().in(Blog::getId, blogIds)
                .last("ORDER BY FIELD(id," + idStr + ")"));
        for (Blog blog : blogs) {
            queryBlogUser(blog);
            isBlogLiked(blog);
        }
        ScrollResult scrollResult = new ScrollResult();
        scrollResult.setList(blogs);
        scrollResult.setOffset(os);
        scrollResult.setMinTime(minTime);

        return Result.ok(scrollResult);


    }

    private void queryBlogUser(Blog blog) {
        Long userId = blog.getUserId();
        User user = userService.getById(userId);
        blog.setName(user.getNickName());
        blog.setIcon(user.getIcon());
    }

}
