#ifndef DKSVARDEF_H
#define DKSVARDEF_H

const int LIMIT_MIN = 1 << 0;
const int LIMIT_MAX = 1 << 1;

enum CMD_RET {
				CR_OK,
				CR_NOTSUPPORTED,
				CR_NOSUCHVAR,
				CR_INVALIDARGS
			 };

#endif