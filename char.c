#include <stdio.h>

int main() {
  char c;
  while (c != '\n') {
    scanf("%c", &c);
    if (c != ' ' && c != '\n') printf("%c", c);
  }

  return 0;
}
