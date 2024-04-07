#include "add.h"

int *

add_1_svc(numbers *argp, struct svc_req *rqstp)

{

static int result;

printf("add(%d,%d) is called\n", argp->a,argp->b);

result = argp->a + argp->b;

return &result;

}
