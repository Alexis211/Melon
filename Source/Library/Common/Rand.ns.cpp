#include "Rand.ns.h"

namespace Rand {

u32int m = 2073741978, a = 50023, b = 1534097;
u64int current = RANDOM_SEED;

u64int rand() {
	current = (u32int)(a*current + b);
	if (current > m) current = current % m;
	return current;
}

u64int max() {
	return m;
}

}
