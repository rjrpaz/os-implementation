/* Automated test program for Project 6
 *
 * Iulian Neamtiu
 *
 */

#include <conio.h>
#include <mq.h>

int doTest( char * testName, int (*testFunction)(), int points, int * score, int * totalTests, int * successfulTests)
{
  int ret;

  (*totalTests)++;

  Print("Testing: %s...", testName);

  ret = testFunction();

  if (ret < 0 )
    Print ("FAILED (%d)", ret);
  else
    {
      Print ("PASSED (%d)", ret);
      (*score) += points;
      (*successfulTests)++;
    }

  Print (" crt score: %d \n", (*score));

  return ret;

}

int tBasicCreateDestroy()
{
  int mqID, retD;

  mqID = Message_Queue_Create("mailbox_1", 100);
  if (mqID < 0)
    return -1;

  retD = Message_Queue_Destroy(mqID);

  return (retD >= 0) ? 1 : -1;
}

int tCreateBigSize()
{
  int retC;

  retC = Message_Queue_Create("mailbox_big", 1000000);

  return (retC < 0) ? 1 : -1;
}

int tReadFromUnopened()
{
  int retR;
  char buffer[100];

  retR = Message_Queue_Receive( 0, buffer, 10);

  return (retR < 0)  ? 1 : -1;
}

int tWriteToUnopened()
{
  int retW;
  char buffer[100];

  retW = Message_Queue_Send( 0, buffer, 10);

  return (retW < 0)  ? 1 : -1;
}

int tDestroyTwice()
{
  int mqID, retD;

  mqID = Message_Queue_Create("mailbox_3", 100);
  if (mqID < 0)
    return -1;

  retD = Message_Queue_Destroy(mqID);
  if (retD < 0)
    return -1;

  retD = Message_Queue_Destroy(mqID);

  return (retD < 0) ? 1 : -1;
}

int tDestroyUnopened()
{
  int retD;

  retD = Message_Queue_Destroy(100000);

  return (retD < 0)  ? 1 : -1;
}


#define BASIC_BUFSIZE 50

int tBasicReadWrite()
{
  int mqID, retD, retW, retR, i;
  char bufferR[BASIC_BUFSIZE], bufferW[BASIC_BUFSIZE];

  mqID = Message_Queue_Create("mailbox_2", 100);
  if (mqID < 0)
    return -1;

  for (i = 0; i < BASIC_BUFSIZE; i++ )
    {
      bufferW[i] = i & 0xff;
      bufferR[i] = 0;
    }

  retW = Message_Queue_Send( mqID, bufferW, BASIC_BUFSIZE);
  if (retW < 0)
    return -1;

  retR = Message_Queue_Receive( mqID, bufferR, BASIC_BUFSIZE);
  if (retR < 0)
    return -1;

  for (i = 0; i < BASIC_BUFSIZE; i++ )
    {
      if (bufferW[i] != bufferR[i])
	{
	  Message_Queue_Destroy(mqID);
	  return -1;
	}
    }


  retD = Message_Queue_Destroy(mqID);

  return (retD >= 0) ? 1 : -1;
}

#define OVERFLOW_BUFSIZE (2 * MESSAGE_MAX_SIZE)

int tWriteOverflow()
{
  int mqID, retD, retW;
  char bufferW[1];

  mqID = Message_Queue_Create("mailbox_wont_fit", MQUEUE_MAX_SIZE);
  if (mqID < 0)
    return -1;

  retW = Message_Queue_Send( mqID, bufferW, OVERFLOW_BUFSIZE);
  if (retW >= 0)
    return -1;

  retD = Message_Queue_Destroy(mqID);

  return (retD >= 0) ? 1 : -1;
}


#define STRESS_BUFSIZE 1000

int tStressReadWrite()
{
  int mqID, retD, retW, retR, i;
  char buffer[STRESS_BUFSIZE];

  mqID = Message_Queue_Create("mailbox_4", (STRESS_BUFSIZE * 2));
  if (mqID < 0)
    return -1;

  for (i = 0; i < 100; i++ )
    {
      retW = Message_Queue_Send( mqID, buffer, STRESS_BUFSIZE);
      if (retW < 0)
	{
	  Message_Queue_Destroy(mqID);
	  return -1;
	}

      retR = Message_Queue_Receive( mqID, buffer, STRESS_BUFSIZE);
      if (retR < 0)
	{
	  Message_Queue_Destroy(mqID);
	  return -1;
	}
    }

  retD = Message_Queue_Destroy(mqID);

  return (retD >= 0) ? 1 : -1;

}


int main(int argc, char **argv)
{


  int score = 0; int totalTests = 0; int successfulTests = 0;

  // 
  doTest( "Basic Create/Destroy", tBasicCreateDestroy, 10, &score, &totalTests, &successfulTests);
  // 
  doTest( "Create with Big Size", tCreateBigSize, 3, &score, &totalTests, &successfulTests);
  // 
  doTest( "Read from Unopened", tReadFromUnopened, 3, &score, &totalTests, &successfulTests);
  // 
  doTest( "Write to Unopened", tWriteToUnopened, 3, &score, &totalTests, &successfulTests);
  // 
  doTest( "Destroy Twice", tDestroyTwice, 3, &score, &totalTests, &successfulTests);
  // 
  doTest( "Destroy Unopened", tDestroyUnopened, 3, &score, &totalTests, &successfulTests);
  // 
  doTest( "Basic Read/Write", tBasicReadWrite, 15, &score, &totalTests, &successfulTests);
  // 
  doTest( "Write Overflow", tWriteOverflow, 5, &score, &totalTests, &successfulTests);
  // 
  doTest( "Stress Read/Write", tStressReadWrite, 15, &score, &totalTests, &successfulTests);



  Print ("********************************************\n");
  Print ("Tests attempted: %d. Passed: %d. Failed: %d\n", totalTests, successfulTests, (totalTests-successfulTests) );
  Print ("SCORE: %d\n", score);

  return 0;
}
