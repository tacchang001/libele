#pragma once
/**
 * @file
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ELE_LOG_STRING_LENGTH_MAX
#define ELE_LOG_STRING_LENGTH_MAX	2048
#endif

/**
 * ログの重要度
 *
 * @note syslogのプライオリティと同等である。
 */
typedef enum {
	kLogLevelEmergency,	// システムが使用不可
	kLogLevelAlert,	// 直ちに行動を起こさなければならない
	kLogLevelCtitical,	// 危険な状態
	kLogLevelError,	// エラーの状態
	kLogLevelWarning,	// ワーニングの状態
	kLogLevelNotice,	// 通常だが重要な状態
	kLogLevelInfo,	// インフォメーションメッセージ
	kLogLevelDebug,	// デバッグレベルのメッセージ
} ele_log_level_t;
#define ELE_LOG_LEVEL_NUM (kLogLevelDebug + 1)

/**
 * ログレベルの考え方（例）
 *
 * kLogLevelEmergencyとkLogLevelAlertは、システムが所望の
 * 機能・性能を発揮できない異常な状態に移入したことを表すログ。
 * kLogLevelEmergencyは、システムが意図せず使用不可に
 * 陥ってしまったことを示すログ。
 * kLogLevelAlertは、放置しておくとシステムが使用不可に
 * 陥る可能性のある状態になったことを示すログ。
 *
 * kLogLevelCtitical、kLogLevelError、kLogLevelWarning、
 * 及びkLogLevelNoticeは入力データや事象に関する情報。
 * kLogLevelCtiticalは、外部からの不正アクセス等明らかに
 * システムを使用不能に陥らせる入力データや事象に関する
 * ログ。
 * kLogLevelErrorは、動作仕様に違反する、あるいは判別不能な
 * 情報や事象に関するログ。
 * kLogLevelWarningは、動作仕様に違反する、あるいは
 * 判別不能な情報や事象ではあるが、対処として無視すること
 * が許されている場合のログ。
 * kLogLevelNoticeは、動作仕様どおりの情報や事象のうち、
 * 得に重要なものに関するログ。又、危険な状態、異常な状態
 * を脱したことを示すログ。
 *
 * kLogLevelInfoとkLogLevelDebugは、それ以外のログに関する
 * 補足情報。あるいは単に詳細な情報。
 * kLogLevelInfoは、開発者以外にも必要な情報。
 * kLogLevelDebugは、開発者にのみ必要な情報。
 *
 * - kLogLevelEmergency
 *   # 動作に必須のNICが装着されていない/外された
 *   # 使用できるメモリ容量が枯渇し処理ができなくなった
 * - kLogLevelAlert
 *   # 使用できるメモリ容量が少なくなり続けており、警告
 *     レベルを越えた。
 *   # ディスク残量が規定の10%を下回った
 * - kLogLevelCtitical
 *   # アクセス権限の無いユーザがログインを試みた
 *   # パスワードを3回以上間違えた
 *   # 識別不能な装置が接続を試みた
 * - kLogLevelError
 *   # 仕様に違反するデータを受信したので
 *     一旦処理を最初からやり直した
 * - kLogLevelWarning
 *   # 仕様に違反するデータを受信したので
 *     破棄して処理を続けた
 * - kLogLevelNotice
 *   # 通常想定しているユーザの入力があった
 *   # NICが装着されて処理を再開した
 *   # メモリ容量が規定容量に戻った
 */

typedef void (*ele_log_handler_t)(
	ele_log_level_t,
	const char*,
	const char*,
	unsigned int,
	const char*
);

/**
 *
 */
void ele_log_string(
	ele_log_level_t level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const char *fmt,
	...
);

/**
 *
 */
void ele_vlog_string(
	ele_log_level_t level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const char *fmt,
	va_list args
);

/**
 *
 */
void ele_log_dump(
	ele_log_level_t level,
	const char *file_name,
	const char *func_name,
	unsigned int line_no,
	const void *top,
	const size_t length
);

/**
 *
 */
int ele_log_set_handler(
	ele_log_level_t level,
	ele_log_handler_t log_func
);

/**
 *
 */
void ele_log_remove_handler(
	ele_log_level_t level,
	int id
);

/**
 *
 */
void ele_log_set_default_handler(
	ele_log_level_t level
);

#define EleLogEmergency(fmt, ...)	ele_log_string(kLogLevelEmergency, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogAlert(fmt, ...)		ele_log_string(kLogLevelAlert, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogCritical(fmt, ...)	ele_log_string(kLogLevelCtitical, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogError(fmt, ...)		ele_log_string(kLogLevelError, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogWarning(fmt, ...)	ele_log_string(kLogLevelWarning, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogNotice(fmt, ...)		ele_log_string(kLogLevelNotice, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define EleLogInfo(fmt, ...)		ele_log_string(kLogLevelInfo, __FILE__, __func__,__LINE__, fmt, ##__VA_ARGS__)
#if defined(DEBUG)
#define EleLogDebug(fmt, ...)		ele_log_string(kLogLevelDebug, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define EleLogDebug(fmt, ...)		/* NOP */
#endif

#define EleDumpEmergency(ptr, len)	ele_log_dump(kLogLevelEmergency, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpAlert(ptr, len)		ele_log_dump(kLogLevelAlert, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpCritical(ptr, len)	ele_log_dump(kLogLevelCtitical, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpError(ptr, len)		ele_log_dump(kLogLevelError, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpWarning(ptr, len)	ele_log_dump(kLogLevelWarning, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpNotice(ptr, len)	ele_log_dump(kLogLevelNotice, __FILE__, __func__, __LINE__, ptr, len)
#define EleDumpInfo(ptr, len)		ele_log_dump(kLogLevelInfo, __FILE__, __func__,__LINE__, ptr, len)
#if defined(DEBUG)
#define EleDumpDebug(ptr, len)		ele_log_dump(kLogLevelDebug, __FILE__, __func__, __LINE__, ptr, len)
#else
#define EleDumpDebug(ptr, len)		/* NOP */
#endif

#ifdef __cplusplus
}
#endif
