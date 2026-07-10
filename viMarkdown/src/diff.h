//----------------------------------------------------------------------
//
//			File:			"diff.h"
//			Created:		11-7-2026
//			Author:			vivisuke
//			Description:
//
//----------------------------------------------------------------------

#pragma once

struct DiffRange {
    int start;
    int length;
};

class DiffBlockUserData : public QTextBlockUserData {
public:
    QList<DiffRange> ranges;
};

