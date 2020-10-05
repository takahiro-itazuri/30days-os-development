/* タイマ関係 */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

void init_pit(void)
{
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff; /* 最初は作動中のタイマがないので */
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = TIMER_UNUSE; /* 未使用 */
	}
	t = timer_alloc(); /* 一つもらってくる */
	t->timeout = 0xffffffff;
	t->flags = TIMER_USING;
	t->next = 0; /* 一番後ろ */
	timerctl.t0 = t; /* 今は番兵しかいないので先頭でもある */
	timerctl.next = 0xffffffff; /* 番兵しかいないので番兵の時刻 */
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == TIMER_UNUSE) {
			timerctl.timers0[i].flags = TIMER_ALLOC;
			timerctl.timers0[i].app_flag = 0;
			return &timerctl.timers0[i];
		}
	}
	return 0; /* 見当たらなかった */
}

void timer_free(struct TIMER *timer)
{
	timer->flags = TIMER_UNUSE; /* 未使用 */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_USING;
	e = io_load_eflags();
	io_cli();

	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		/* 先頭に入れる場合 */
		timerctl.t0 = timer;
		timer->next = t; /* 次はt */
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	/* どこに入れればいいかを探す */
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			/* sとtの間に入れる場合 */
			s->next = timer; /* sの次はtimer */
			timer->next = t; /* timerの次はt */
			io_store_eflags(e);
			return;
		}
	}
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 設定中にタイマの状態が変化しないようにするため */
	if (timer->flags & TIMER_USING) {	/* 取り消し処理は必要か？ */
		if (timer == timerctl.t0) {
			/* 先頭だった場合の取り消し処理 */
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		} else {
			/* 先頭以外の場合の取り消し処理 */
			/* timerの一つ前を探す */
			t = timerctl.t0;
			for (;;) {
				if (t->next == timer) {
					break;
				}
				t = t->next;
			}
			t->next = timer->next; /* 「timerの直前」の次が、「timerの次」を指すようにする */
		}
		timer->flags = TIMER_ALLOC;
		io_store_eflags(e);
		return 1;	/* キャンセル処理成功 */
	}
	io_store_eflags(e);
	return 0; /* キャンセル処理は不要だった */
}

void timer_cancelall(struct FIFO32 *fifo)
{
	int e, i;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();	/* 設定中にタイマの状態が変化しないようにするため */
	for (i = 0; i < MAX_TIMER; i++) {
		t = &timerctl.timers0[i];
		if (t->flags != TIMER_UNUSE && t->app_flag && t->fifo == fifo) {
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	struct TIMER *timer;
	char ts = 0;
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00受付完了をPICに通知 */
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return; /* まだ次の時刻になってないので、もうおしまい */
	}
	timer = timerctl.t0; /* とりあえず先頭の番地をtimerに代入 */
	for (;;) {
		/* timersのタイマは全て動作中のものなので、flagsを確認しない */
		if (timer->timeout > timerctl.count) {
			break;
		}
		/* タイムアウト */
		timer->flags = TIMER_ALLOC;
		if (timer != task_timer) {
			fifo32_put(timer->fifo, timer->data);
		}
		else {
			ts = 1; /* mt_timerがタイムアウトした */
		}
		timer = timer->next; /* 次のタイマの番地をtimerに代入 */
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if (ts != 0) {
		task_switch();
	}
	return;
}
