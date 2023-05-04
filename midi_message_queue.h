#ifndef _MIDI_MESSAGE_QUEUE_H
#define _MIDI_MESSAGE_QUEUE_H

#define MIDI_MESSAGE_QUEUE_MAX_SIZE 1024

#include <pico/multicore.h>

/*
 * Midi Message
 */
typedef struct {
  uint64_t timestamp;
  uint8_t status;
  uint8_t data1;
  uint8_t data2;
} midi_message_t;

/*
 * Midi Message Queue
 */
typedef struct {
  midi_message_t data[MIDI_MESSAGE_QUEUE_MAX_SIZE];
  uint32_t front;
  uint32_t rear;
  spin_lock_t *lock;
} midi_message_queue;

static inline void midi_message_queue_init(midi_message_queue *q) {
  q->front = 0;
  q->rear = 0;

  int lock_num = spin_lock_claim_unused(true);
  q->lock = spin_lock_init(lock_num);
}

static inline bool midi_message_queue_is_empty(midi_message_queue *q) {
  return q->front == q->rear;
}

static inline bool midi_message_queue_is_full(midi_message_queue *q) {
  return ((q->rear + 1) % MIDI_MESSAGE_QUEUE_MAX_SIZE) == q->front;
}

static inline uint32_t midi_message_queue_count(midi_message_queue *q) {
  if (q->front > q->rear)
    return MIDI_MESSAGE_QUEUE_MAX_SIZE - (q->front - q->rear);
  return q->rear - q->front;
}

static inline bool midi_message_queue_enqueue(midi_message_queue *q,
                                              midi_message_t msg) {
  spin_lock_unsafe_blocking(q->lock);
  if (midi_message_queue_is_full(q)) {
    spin_unlock_unsafe(q->lock);
    return false;
  }

  q->rear = (q->rear + 1) % MIDI_MESSAGE_QUEUE_MAX_SIZE;
  q->data[q->rear] = msg;
  spin_unlock_unsafe(q->lock);
  return true;
}

static inline bool midi_message_queue_dequeue(midi_message_queue *q,
                                              midi_message_t *msg) {
  spin_lock_unsafe_blocking(q->lock);
  if (midi_message_queue_is_empty(q)) {
    spin_unlock_unsafe(q->lock);
    return false;
  }

  q->front = (q->front + 1) % MIDI_MESSAGE_QUEUE_MAX_SIZE;
  midi_message_t ret = q->data[q->front];
  spin_unlock_unsafe(q->lock);
  *msg = ret;
  return true;
}

#endif
