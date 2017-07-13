
/*
 * atoi - only works for unsigned values
 *
 */

int atoi(char *buf)
{
    int ret = 0;

    while (*buf >= '0' && *buf <= '9') {
       ret *= 10;
       ret += *buf - '0';
       buf++;
    }

    return ret;
}

