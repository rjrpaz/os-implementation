/*
 * Message queues system calls header
 * Copyright (c) 2004, David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision$
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_MQUEUE_H
#define GEEKOS_MQUEUE_H

#define MQUEUE_MAX_SIZE 4096

/* in theory, there's no reason to limit MESSAGE_MAX_SIZE,
   but we want to avoid pathologically large mesaages */
#define MESSAGE_MAX_SIZE (2 * MQUEUE_MAX_SIZE)

#endif  /* GEEKOS_MQUEUE_H */
