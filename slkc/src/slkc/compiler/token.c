#include <slkc/compiler/token.h>

skToken sk_make_token(skETokenType type, usize line)
{
	skToken token;
	token.type = type;
	token.line = line;
	token.value_int = 0;
	return token;
}
