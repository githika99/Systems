#include "rwlock.h"

#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
  printf("\nEntered main function");
  (void)argc;
  (void)argv;

  printf("\nAbout to create rwlock obj");
  rwlock_t *rw = rwlock_new(READERS, -1);
  printf("\nCreated rwlock obj");

  if (rw == NULL) {
    printf("\nFailed to create rwlock obj");
    return 1;
  }

  printf("\nAbout to get reader lock!");
  reader_lock(rw);
  printf("\nI got a reader lock!, unlocking now");
  reader_unlock(rw);
  printf("\nUnlocked reader!");

  printf("\nAbout to get writer lock!");
  writer_lock(rw);
  printf("\nI got a writer lock!, unlocking now");
  writer_unlock(rw);
  printf("\nUnlocked reader!");

  printf("\nAbout to delete lock");
  rwlock_delete(&rw);
  printf("\nDeleted lock!");
  return 0;
}
