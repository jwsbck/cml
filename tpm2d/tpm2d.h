/*
 * This file is part of trust|me
 * Copyright(c) 2013 - 2017 Fraunhofer AISEC
 * Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 (GPL 2), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GPL 2 license for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information:
 * Fraunhofer AISEC <trustme@aisec.fraunhofer.de>
 */

#ifndef TPM2D_H
#define TPM2D_H

#include <tss2/tss.h>

#define TPM2D_ASYM_ALGORITHM		TPM_ALG_ECC
#define TPM2D_CURVE_ID			TPM_ECC_NIST_P256
#define TPM2D_HASH_ALGORITHM		TPM_ALG_SHA256
#define TPM2D_SYM_SESSION_ALGORITHM 	TPM_ALG_AES

#define TPM2D_KEY_HIERARCHY		TPM_RH_OWNER
#define TPM2D_FDE_NV_HANDLE		0x01000000

#define TPM2D_BASE_DIR "/data/cml/tpm2d"
#define TPM2D_SESSION_DIR "session"

#ifndef TPM2D_NVMCRYPT_ONLY

#define TPM2D_TOKEN_DIR "tokens"

#define TPM2D_PLATFORM_STORAGE_KEY_PERSIST_HANDLE	0x81800000
#define TPM2D_OWNER_STORAGE_KEY_PERSIST_HANDLE		0x81000000

// TODO proper auth handling (generate pws during provisioning)
#define TPM2D_PRIMARY_STORAGE_KEY_PW	"primary"
#define TPM2D_ATTESTATION_KEY_PW	"sig"

#define TPM2D_ATTESTATION_PRIV_FILE	TPM2D_BASE_DIR "/" TPM2D_TOKEN_DIR "/attestation_priv.bin"
#define TPM2D_ATTESTATION_PUB_FILE	TPM2D_BASE_DIR "/" TPM2D_TOKEN_DIR "/attestation_pub.bin"
#define TPM2D_FDE_PRIV_FILE		TPM2D_BASE_DIR "/" TPM2D_TOKEN_DIR "/fde_priv.bin"
#define TPM2D_FDE_PUB_FILE		TPM2D_BASE_DIR "/" TPM2D_TOKEN_DIR "/fde_pub.bin"
#define TPM2D_PS_PUB_FILE		TPM2D_BASE_DIR "/" TPM2D_TOKEN_DIR "/ps_pub.bin"

typedef struct tpm2d_quote_string {
	char *halg_str;
	char *quoted_str;
	char *signature_str;
} tpm2d_quote_string_t;

#endif // ndef TPM2D_NVMCRYPT_ONLY

typedef enum tpm2d_key_type {
	TPM2D_KEY_TYPE_STORAGE_U = 1,
	TPM2D_KEY_TYPE_STORAGE_R,
	TPM2D_KEY_TYPE_SIGNING_U,
	TPM2D_KEY_TYPE_SIGNING_R
} tpm2d_key_type_t;

typedef struct tpm2d_pcr_string {
	char *halg_str;
	char *pcr_str;
} tpm2d_pcr_string_t;

/*****************************************************************************/
void
tpm2d_exit(void);

TPMI_DH_OBJECT
tpm2d_get_salt_key_handle(void);

void
tss2_init(void);

void
tss2_destroy(void);

/**
 * Helper function to convert a binary buffer to an hex string
 *
 * This function allocates a new buffer containing the
 * resulting hex representation containing a terminating '\0'
 * of the binary buffer.
 *
 * @param bin binary buffer
 * @param length of the binary buffer
 */
char *
convert_bin_to_hex_new(const uint8_t *bin, int length);

/**
 * Helper function to convert a hex string into binary
 *
 * This function allocates a new buffer containing the
 * resulting binary representation of the string.
 *
 * @param hex_str buffer containing the hex string representation
 * @param out_length in this pointer the size of result is returned
 */
uint8_t *
convert_hex_to_bin_new(const char *hex_str, int *out_length);

/**
 * Function to powerup the simulator
 *
 * This function only may used for simulator. On a real TPM this
 * is done in hardware and would fail.
 */
TPM_RC
tpm2_powerup(void);

/**
 * Funtion to startup the TPM.
 *
 * This function is only needed for simulator. In case of a real
 * TPM the BIOS/UEFI has already started the TPM.
 *
 * @param startup_type type value for startup
 */
TPM_RC
tpm2_startup(TPM_SU startup_type);

/**
 * Funtion to check if TPM is usable.
 */
TPM_RC
tpm2_selftest(void);

/**
 * Function to generate the primary key of an hierarchy
 */
TPM_RC
tpm2_createprimary_asym(TPMI_RH_HIERARCHY hierachy, tpm2d_key_type_t key_type,
		const char *hierachy_pwd, const char *key_pwd,
		const char *file_name_pub_key, uint32_t *out_handle);

#ifndef TPM2D_NVMCRYPT_ONLY
TPM_RC
tpm2_create_asym(TPMI_DH_OBJECT parent_handle, tpm2d_key_type_t key_type,
		uint32_t object_vals, const char *parent_pwd, const char *key_pwd,
		const char *file_name_priv_key, const char *file_name_pub_key);

TPM_RC
tpm2_load(TPMI_DH_OBJECT parent_handle, const char *parent_pwd,
		const char *file_name_priv_key, const char *file_name_pub_key,
		uint32_t *out_handle);

TPMI_DH_OBJECT
tpm2d_get_as_key_handle(void);

TPM_RC
tpm2_pcrextend(TPMI_DH_PCR pcr_index, TPMI_ALG_HASH hash_alg, const char *data);

tpm2d_pcr_string_t *
tpm2_pcrread_new(TPMI_DH_PCR pcr_index, TPMI_ALG_HASH hash_alg);

void
tpm2_pcrread_free(tpm2d_pcr_string_t *pcr_string);

tpm2d_quote_string_t *
tpm2_quote_new(TPMI_DH_PCR pcr_indices, TPMI_DH_OBJECT sig_key_handle,
			const char *sig_key_pwd, const char *qualifying_data);

void
tpm2_quote_free(tpm2d_quote_string_t *quote_string);

char *
tpm2_read_file_to_hex_string_new(const char *file_name);

TPM_RC
tpm2_evictcontrol(TPMI_RH_HIERARCHY auth, char* auth_pwd, TPMI_DH_OBJECT obj_handle,
						 TPMI_DH_PERSISTENT persist_handle);

TPM_RC
tpm2_rsaencrypt(TPMI_DH_OBJECT key_handle, uint8_t *in_buffer, size_t in_length,
                         uint8_t *out_buffer, size_t *out_length);

TPM_RC
tpm2_rsadecrypt(TPMI_DH_OBJECT key_handle, const char *key_pwd, uint8_t *in_buffer,
			size_t in_length, uint8_t *out_buffer, size_t *out_length);
#endif // ndef TPM2D_NVMCRYPT_ONLY

uint8_t *
tpm2_getrandom_new(size_t rand_length);

TPM_RC
tpm2_nv_definespace(TPMI_RH_HIERARCHY hierarchy, TPMI_RH_NV_INDEX nv_index_handle,
		size_t nv_size, const char *hierarchy_pwd, const char *nv_pwd);

TPM_RC
tpm2_nv_write(TPMI_RH_NV_INDEX nv_index_handle, const char *nv_pwd,
					uint8_t *data, size_t data_length);

TPM_RC
tpm2_nv_read(TPMI_RH_NV_INDEX nv_index_handle, const char *nv_pwd,
				uint8_t *out_buffer, size_t *out_length);

#endif // TPM2D_H
