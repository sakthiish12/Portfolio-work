// /******************************************************************************/
// /*!
// \file         Assert.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         09th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Assert only for debugging
// \lines        19 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef PARTYASSERT_H
#define PARTYASSERT_H

#ifndef _DEBUG
inline void PartyAssert(bool, const char*) { (void)0; } /* Remove asserted code during Release mode */
#else
#include "Log.h"
#define PartyAssert(condition, loggedMessage) /* Logs a Error message into a file if condition is FALSE */ \
		{\
			if (!(condition))\
			{\
				std::stringstream _partyAssertStringStream_; \
				_partyAssertStringStream_ << "ASSERT FAILED: " << #loggedMessage << " @ " << __FILE__ << " (" << __LINE__ << ")\n"; \
				Log _partyAssertlogger_; \
				_partyAssertlogger_.Error(true, _partyAssertStringStream_.str().c_str()); \
				assert(condition); \
			}\
		}
#endif

#endif