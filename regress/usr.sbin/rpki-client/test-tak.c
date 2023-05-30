/*	$Id: test-tak.c,v 1.5 2023/05/30 12:14:48 claudio Exp $ */
/*
 * Copyright (c) 2022 Job Snijders <job@fastly.com>
 * Copyright (c) 2019 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include "extern.h"

int outformats;
int verbose;
int filemode;

int
main(int argc, char *argv[])
{
	int		 c, i, ppem = 0, verb = 0;
	X509		*xp = NULL;
	struct tak	*p;
	unsigned char	*buf;
	size_t		 len;

	ERR_load_crypto_strings();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	x509_init_oid();

	while ((c = getopt(argc, argv, "pv")) != -1)
		switch (c) {
		case 'p':
			if (ppem)
				break;
			ppem = 1;
			break;
		case 'v':
			verb++;
			break;
		default:
			errx(1, "bad argument %c", c);
		}

	argv += optind;
	argc -= optind;

	if (argc == 0)
		errx(1, "argument missing");

	for (i = 0; i < argc; i++) {
		buf = load_file(argv[i], &len);
		if ((p = tak_parse(&xp, argv[i], buf, len)) == NULL) {
			free(buf);
			break;
		}
		if (verb)
			tak_print(xp, p);
		if (ppem) {
			if (!PEM_write_X509(stdout, xp))
				errx(1, "PEM_write_X509: unable to write cert");
		}
		free(buf);
		tak_free(p);
		X509_free(xp);
	}

	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();

	if (i < argc)
		errx(1, "test failed for %s", argv[i]);

	printf("OK\n");
	return 0;
}

time_t
get_current_time(void)
{
	return time(NULL);
}
