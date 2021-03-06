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
 * Filename:    SCRAM.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: This header defines some helper methods for authenticating
 *              using the Salted Challenge Response (SCRAM) SASL mechanism.
 *              See http://tools.ietf.org/html/draft-ietf-sasl-scram-10
 */

#pragma once
#include "DispInterfaceImpl.h"
#include "StringPrep.h"

class SCRAM :
	public DispInterfaceImpl<ISCRAM>
{
	StringPrep sprep;
public:
	SCRAM();
	~SCRAM();

	STDMETHOD(GenerateResponse)(
		BSTR Challenge,
		BSTR *Response);

private:
	std::vector<unsigned char> SCRAM::HMAC_SHA1(
		const std::string key,
		const std::string text);

	void Error(
		std::wstring location,
		std::wstring whenCalling,
		unsigned errorCode);
};
