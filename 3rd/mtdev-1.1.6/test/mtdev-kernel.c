/*****************************************************************************
 *
 * mtdev - Multitouch Protocol Translation Library (MIT license)
 *
 * Copyright (C) 2010 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2010 Canonical Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 ****************************************************************************/

#include <../src/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Combinatorial formulation
 *
 * x_ij = 1 if slot i and contact j are connected, zero otherwise
 *
 * sum_i x_ij <= 1 for all j; each contact picks at most one slot
 *
 * sum_j x_ij <= 1 for all i; each slot is picked by at most one contact
 *
 * sum_ij x_ij == min(nslot, npos); assign every contact possible
 *
 * Arrange x_ij as a bitmask; x_00 x_01 x_02.. x_10 x_11 x_12...
 *
 * Up to five slots, this is readily enumerable.
 */

#define SLOT_MAX	5
#define SLOT_CNT	(SLOT_MAX + 1)

static int illegal(int nslot, int npos, unsigned x)
{
	int i, j, sum;

	for (j = 0; j < npos; j++) {
		sum = 0;
		for (i = 0; i < nslot; i++)
			sum += GETBIT(x, i * npos + j);
		if (sum > 1)
			return 1;
	}
	for (i = 0; i < nslot; i++) {
		sum = 0;
		for (j = 0; j < npos; j++)
			sum += GETBIT(x, i * npos + j);
		if (sum > 1)
			return 1;
	}

	sum = bitcount(x);
	return sum != minval(nslot, npos);
}

static void get_slots(int *slots, int nslot, int npos, unsigned x)
{
	int i;

	memset(slots, -1, sizeof(slots[0]) * npos);
	for (i = 0; i < nslot * npos; i++)
		if (GETBIT(x, i))
			slots[i % npos] = i / npos;
	for (i = 0; i < npos; i++)
		if (slots[i] < 0)
			slots[i] = nslot++;
}

static int generate_assignments(int nslot, int npos)
{
	static int ncol;
	unsigned x, nx = BITMASK(nslot * npos);
	int slots[SLOT_MAX];
	int i, n = 0;

	for (x = 0; x < nx; x++) {
		if (illegal(nslot, npos, x))
			continue;
		for (i = 0; i < nslot * npos; i++) {
			if (GETBIT(x, i)) {
				if (ncol++ % 16 == 0)
					printf("\n\t%d,", i);
				else
					printf(" %d,", i);
				n++;
			}
		}
		get_slots(slots, nslot, npos, x);
		for (i = 0; i < npos; i++) {
			if (ncol++ % 16 == 0)
				printf("\n\t%d,", slots[i]);
			else
				printf(" %d,", slots[i]);
			n++;
		}
	}

	return n;
}

int main(int argc, char *argv[])
{
	int ix[SLOT_CNT][SLOT_CNT], nix = 0;
	int eslot, i, j;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <num_slots>\n", argv[0]);
		return 1;
	}

	eslot = atoi(argv[1]) + 1;
	if (eslot > SLOT_CNT) {
		fprintf(stderr, "allowed slot range: 2 - %d\n", SLOT_MAX);
		return 1;
	}

	printf("\n/* generated by mtdev-kernel - do not edit */\n");
	printf("static const u8 match_data[] = {");
	for (i = 0; i < eslot; i++) {
		for (j = 0; j < eslot; j++) {
			ix[i][j] = nix;
			nix += generate_assignments(i, j);
		}
	}
	printf("\n};\n");

	printf("\n/* generated by mtdev-kernel - do not edit */\n");
	printf("static const int match_index[][%d] = {\n", eslot);
	for (i = 0; i < eslot; i++) {
		printf("\t{");
		for (j = 0; j < eslot; j++)
			printf(" %d%s", ix[i][j], j < eslot - 1 ? "," : "");
		printf(" },\n");
	}
	printf("\t{ %d }\n", nix);
	printf("};\n");

	return 0;
}
