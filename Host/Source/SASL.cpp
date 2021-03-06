/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    SASL.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"

#include "SASL.h"
#include "Hash.h"
#include "Base64.h"
#include "UTF.h"

SASL::SASL()
{
	m_CannotSelfDelete = true;
}
SASL::~SASL()
{
}
STDMETHODIMP SASL::PlainGenerateResponse(BSTR jid, BSTR username,
	BSTR password, BSTR* strBase64)
{
	std::stringstream stringBuffer;
	stringBuffer <<
		UTF::utf16to8(jid) <<
		'\0' <<
		UTF::utf16to8(username) <<
		'\0' <<
		UTF::utf16to8(password);
	*strBase64 = ::SysAllocString(Base64::Encode(stringBuffer.str().c_str(),
		stringBuffer.str().length(), false).c_str());
	return S_OK;
}
STDMETHODIMP SASL::DigestGenerateResponse(BSTR username, BSTR realm,
	BSTR password, BSTR nonce, BSTR cnonce, BSTR digest_uri, BSTR nc,
	BSTR qop, BSTR *strDigest)
{
	/* MD5-Session algorithm:
	var X	= Response('username') + ':' + Response('realm') + ':' +
		Response('password');
	var Y	= md5.digest(X); // This should be binary data instead of the 
							 // hexadecimal string
	var A1	= Y + ':' + Response('nonce') + ':' +
		Response('cnonce'); // + ':' + Response('authzid')
	var A2	= 'AUTHENTICATE:' + Response('digest-uri');
	var HA1	= md5.digest(A1);
	var HA2	= md5.digest(A2);
	var KD	= HA1 + ':' + Response('nonce') + ':' + Response('nc') + ':' + 
		Response('cnonce') + ':' + Response('qop') + ':' + HA2;
	var Z	= md5.digest(KD);
	*/
	std::string colon = std::string(":");

	/* calculate X */
	std::string X = UTF::utf16to8(username) + colon +
		UTF::utf16to8(realm) + colon + UTF::utf16to8(password);

	/* calculate Y */
	unsigned char binaryY[16];
	Hash::MD5((unsigned char*)X.c_str(), X.length(), binaryY);

	/* calculate A1 */
	std::string A1 = "1234567890123456" + colon + UTF::utf16to8(nonce) +
		colon + UTF::utf16to8(cnonce);
	std::copy(binaryY, binaryY + 16, &A1[0]);

	/* calculate A2 */
	std::string A2 = std::string("AUTHENTICATE:") + UTF::utf16to8(digest_uri);

	/* calculate HA1 */
	unsigned char binaryHA1[16];
	Hash::MD5((unsigned char*)A1.c_str(), A1.length(), binaryHA1);
	std::string HA1(2*16, '\0');
	HexString(binaryHA1, &HA1[0], 16);

	/* calculate HA2 */
	unsigned char binaryHA2[16];
	Hash::MD5((unsigned char*)A2.c_str(), A2.length(), binaryHA2);
	std::string HA2(2*16, '\0');
	HexString(binaryHA2, &HA2[0], 16);

    /* calculate KD */
	std::string KD = HA1 + colon + UTF::utf16to8(nonce) + colon +
		UTF::utf16to8(nc) + colon + UTF::utf16to8(cnonce) + 
		colon +	UTF::utf16to8(qop) + colon + HA2;

	/* calculate Z */
	unsigned char binaryZ[16];
	Hash::MD5((unsigned char*) KD.c_str(), KD.length(), binaryZ);
	std::string Z(2*16, '\0');
	HexString(binaryZ, &Z[0], 16);

	*strDigest = ::SysAllocString(UTF::utf8to16(Z).c_str());

	return S_OK;
}
STDMETHODIMP SASL::get_SCRAM(VARIANT* pDispatch)
{
	pDispatch->vt = VT_DISPATCH;
	return m_SCRAM.QueryInterface(IID_IDispatch, (void**)&pDispatch->pdispVal);
}
STDMETHODIMP SASL::get_SSPI(VARIANT* pDispatch)
{
	pDispatch->vt = VT_DISPATCH;
	return m_SSPI.QueryInterface(IID_IDispatch, (void**)&pDispatch->pdispVal);
}
STDMETHODIMP SASL::get_GSSAPI(VARIANT* pDispatch)
{
	pDispatch->vt = VT_DISPATCH;
	return m_GSSAPI.QueryInterface(IID_IDispatch, (void**)&pDispatch->pdispVal);
}
void SASL::HexString(const unsigned char *binaryData, char *hexString, int n)
{
	for(int i = 0; i < n; i++)
	{
		hexString[2 * i] =
			(binaryData[i] >> 4) + 
			(((binaryData[i] >> 4) < 0xA) ? '0' : ('a' - 0xA));
		hexString[2 * i + 1] = (binaryData[i] & 0x0F) + 
			(((binaryData[i] & 0x0F) < 0xA) ? '0' : ('a' - 0xA));
	}
	hexString[2*n] = '\0';
}
