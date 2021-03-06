/*
 * Copyright (C) 2017 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#import <Foundation/Foundation.h>
#import <TwitterKit/TWTRJSONConvertible.h>

@protocol TWTRVersionedCacheable <NSObject, NSCoding>

/**
 *  Returns the versioned key for instances of the class to read and write this cacheable object into the cache store.
 *  This depends on +[Class<NSObject> version]. Version (default = 0) should be bumped every time we
 *  support a new or update an existing property of the concrete class.
 *
 *  @param IDString     (Required) ID of the instance of the cacheable class
 *  @param perspective  This is typically the currently authenticated user
 *                      but could be any key that differentiates views of the
 *                      data. `nil` means there's only one view.
 */
+ (NSString *_Nonnull)versionedCacheKeyWithID:(NSString *_Nonnull)IDString perspective:(NSString *_Nonnull)perspective __attribute__((nonnull(1)));

@end

@class TWTRTweet;
@class TWTRUser;

NS_ASSUME_NONNULL_BEGIN

/**
 *  `TWTRTweet` is an immutable representation of a Tweet.
 */
@interface TWTRTweet : NSObject <NSCoding, TWTRJSONConvertible>

#pragma mark - Properties

/**
 *  The ID of the Twitter Tweet.
 *  @warning This represents the id_str field, which could differ from the value of the id field.
 */
@property (nonatomic, copy, readonly) NSString *tweetID;

/**
 *  The date when this Tweet was created.
 */
@property (nonatomic, copy, readonly) NSDate *createdAt;

/**
 *  The text of the Tweet.
 */
@property (nonatomic, copy, readonly) NSString *text;

/**
 *  The Author of the Tweet.
 */
@property (nonatomic, readonly) TWTRUser *author;

/**
 *  ID of the authenticated Twitter user this Tweet was loaded for. Some Tweet properties e.g. `isLiked`
 *  can vary depending on the authenticated user. Nil means the Tweet was loaded from the perspective
 *  of a logged-out user or the authenticated user could not be determined.
 */
@property (nonatomic, readonly, nullable) NSString *perspectivalUserID;

/**
 *  The number of times this Tweet was liked.
 */
@property (nonatomic, readonly) long long likeCount;

/**
 *  The number of times this Tweet was retweeted.
 */
@property (nonatomic, readonly) long long retweetCount;

/**
 *  The language of the Tweet.
 */
@property (nonatomic, copy, readonly) NSString *languageCode;

/**
 *  The Tweet this Tweet was a reply to.
 */
@property (nonatomic, copy, readonly, nullable) NSString *inReplyToTweetID;

/**
 *  The User ID this Tweet was a reply to.
 */
@property (nonatomic, copy, readonly, nullable) NSString *inReplyToUserID;

/**
 *  The screen name of the user this Tweet was a reply to.
 *  @note This doesn't contain the `@` sign before the screen name.
 */
@property (nonatomic, copy, readonly, nullable) NSString *inReplyToScreenName;

/**
 *  The permalink URL for this Tweet.
 *
 *  Suitable for loading in a `UIWebView`, `WKWebView` or passing to Safari:
 *
 *  `[[UIApplication sharedApplication] openURL:tweet.permalink];`
 */
@property (nonatomic, copy, readonly) NSURL *permalink;

/**
 *  Whether this Tweet was liked by the authenticated user.
 *
 *  @warning The value of this property depends on the authenticated user.
 */
@property (nonatomic, readonly) BOOL isLiked;

/**
 *  Whether this Tweet was retweeted by the authenticated user.
 *
 *  @warning The value of this property depends on the authenticated user.
 */
@property (nonatomic, readonly) BOOL isRetweeted;

/**
 *  The Tweet ID of the authenticated user's retweet of this Tweet. This will be `nil` if there is no
 *  authenticated user or the user has not retweeted this Tweet.
 *
 *  @warning The value of this property depends on the authenticated user.
 */
@property (nonatomic, copy, readonly, nullable) NSString *retweetID;

/**
 *  The original, fully-hydrated Tweet that was retweeted. This corresponds to the `retweeted_status` API field.
 *  This is `nil` unless `self.isRetweet == YES`.
 */
@property (nonatomic, readonly, nullable) TWTRTweet *retweetedTweet;

/**
 *  Indicates whether this Tweet is a retweet of another Tweet.
 */
@property (nonatomic, readonly) BOOL isRetweet;

/**
 * Indicates whether this Tweet is a Quote Tweet.
 */
@property (nonatomic, readonly) BOOL isQuoteTweet;

/**
 *  The original, fully-hydrated Tweet that was quoted.
 *  This is `nil` unless `self.isRetweet == YES`.
 */
@property (nonatomic, readonly, nullable) TWTRTweet *quotedTweet;

/**
 *  Creates an array of TWTRTweet instances from the array of Twitter API JSON response.
 *
 *  @param array A parsed array of Tweet API JSON responses.
 *  @return An array of TWTRTweet instances.
 */
+ (NSArray *)tweetsWithJSONArray:(nullable NSArray *)array;

/**
 *  Creates a new Tweet instance with a new value for the `isLiked` boolean
 *  value which is the opposite of the current value.
 */
- (TWTRTweet *)tweetWithLikeToggled;

/**
 *  Tweet objects should be hyrdrated from a valid JSON object. See TWTRJSONConvertible for more information.
 */
- (instancetype)init NS_UNAVAILABLE;

@end

@class TWTRCardEntity;
@class TWTRTweetMediaEntity;
@class TWTRVideoMetaData;
@class TWTRTweetRepository;


FOUNDATION_EXTERN NSString *const TWTRCompactTweetExpandedURLString;
FOUNDATION_EXTERN NSString *const TWTRTweetPerspectivalUserID;

@interface TWTRTweet () <TWTRVersionedCacheable>

@property (nonatomic, readonly) TWTRTweetRepository *tweetRepo;

#pragma mark - Private Properties

@property (nonatomic, copy, readonly, nullable) NSArray *hashtags;
@property (nonatomic, copy, readonly, nullable) NSArray *cashtags;
@property (nonatomic, copy, readonly, nullable) NSArray<TWTRTweetMediaEntity *> *media;
@property (nonatomic, copy, readonly, nullable) NSArray *urls;
@property (nonatomic, copy, readonly, nullable) NSArray *userMentions;

@property (nonatomic, readonly, nullable) TWTRCardEntity *cardEntity;

@property (nonatomic, readonly, nullable) TWTRVideoMetaData *videoMetaData;

#pragma mark - Getters and Setters

/**
 * Returns true if the Tweet has media entities.
 */
- (BOOL)hasMedia;

/**
 * Returns true if the Tweet has a media entity which has associated video or
 * the card entity contains playable media.
 */
- (BOOL)hasPlayableVideo;

/**
 *  Returns true if the Tweet has a card entity attached
 *  which is a Vine card.
 */
- (BOOL)hasVineCard;

/**
 *  Returns a new Tweet with the perspectival user ID set. This data is only available when fetching
 *  Tweets with `TWTRAPIClient` since the REST API does not include the authenticated user making
 *  the request.
 *
 *  @param userID ID of the Twitter user who fetched this Tweet. Nil means logged-out user.
 *
 *  @return Copy of the Tweet with the `perspectivalUserID` set to the given ID.
 */
- (TWTRTweet *)tweetWithPerspectivalUserID:(nullable NSString *)userID;

@end

NS_ASSUME_NONNULL_END
