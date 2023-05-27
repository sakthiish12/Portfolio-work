#include "pch.h"
#include "Utility.h"

namespace BE
{
	//Helper function to convert filepath slash from \\ to /
	void ConvertFilePathSlash(std::string& _fp)
	{
		for (size_t i = 0; i < _fp.size(); ++i)
		{
			if (_fp[i] == '\\')
			{
				_fp[i] = '/';
				if (i < _fp.size() && _fp[i + 1] == '\\')
					_fp.erase(i + 1, 1);
			}
		}
	}

	//Helper function to truncate filepath to the last part
	void TruncateFilePath(std::string& _fp)
	{
		size_t pos = _fp.find_last_of("\\/");

		if (pos <= _fp.size())
			_fp.erase(0, pos + 1);
	}
}