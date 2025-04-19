#include "LogicDataAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "LogicDataAnalyzer.h"
#include "LogicDataAnalyzerSettings.h"
#include <iostream>
#include <fstream>

LogicDataAnalyzerResults::LogicDataAnalyzerResults( LogicDataAnalyzer* analyzer, LogicDataAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

LogicDataAnalyzerResults::~LogicDataAnalyzerResults()
{
}

const char *LogicDataAnalyzerResults::FormatCommand(U16 command) {
	switch (command) {
		case 0x200:
			return "Height";
		case 0x281:
			return "Exit Settings";
		case 0x282:
			return "Enter Settings";
		case 0x287:
			return "Display Off";
		case 0x288:
			return "Display On";
		case 0x380:
			return "Display Clear";
		case 0x383:
			return "Setting Selected";
		case 0x389:
			return "Error Report";
		case 0x392:
			return "Status Report";
		default:
			return NULL;
	}
}

const char *LogicDataAnalyzerResults::FormatDisplay(U16 command, U8 data, char *buf, size_t buflen) {
	switch (command) {
		case 0x200:
			// Height report in cm
			snprintf(buf, buflen, "%d", data);
			break;
		case 0x282:
			// Enter settings
			snprintf(buf, buflen, "S-");
			break;
		case 0x380:
			// Display clear?
			snprintf(buf, buflen, "---");
			break;
		case 0x383:
			// Setting selected
			snprintf(buf, buflen, "S-%d", data);
			break;
		case 0x389:
			// Error report
			snprintf(buf, buflen, "E-%d", data);
			break;
		default:
			buf[0] = '\0';
			break;
	}
	return buf;
}

bool LogicDataAnalyzerResults::ValidWord(U32 word, U16 *command, U8 *data) {
	bool result = false;
	if ((word & 0x00000FFF) == (0x00000602)) {
		*command = ((word & 0x007FF000) >> 12);
		*data = ((word & 0x7F800000) >> 23);
		result = true;
	}
	return result;
}

void LogicDataAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char str_number[16];
	char str_display[10];
	char str_result[128];

	if (frame.mType == 1) {
		AddResultString( "SYNC" );
	}
	if (frame.mType == 2) {
		snprintf(str_number, sizeof(str_number), "0x%08x", (U32)frame.mData1);
		U16 command;
		U8 data;
		if (ValidWord(frame.mData1, &command, &data)) {
			FormatDisplay(command, data, str_display, sizeof(str_display));
			if (strlen(str_display) > 0) {
				snprintf(str_result, sizeof(str_result), "%s - %X:%X - \"%s\"", str_number, command, data, str_display);
			} else {
				const char *command_str = FormatCommand(command);
				if (command_str) {
					snprintf(str_result, sizeof(str_result), "%s - %X:%X - %s", str_number, command, data, command_str);
				} else {
					snprintf(str_result, sizeof(str_result), "%s - %X:%X", str_number, command, data);
				}
			}
			AddResultString(str_result);
		} else {
			// Doesn't look like a valid word - we can't decode
			AddResultString(str_number);
		}
	}
}

void LogicDataAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Word,Cmd,Cmd Type,Data,Display" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		// if (frame.mType == 1) {
		// 	file_stream << time_str << "," << "SYNC" << std::endl;
		// }
		if (frame.mType == 2) {

			char str_number[16];
			char str_display[10];
			char str_cmd[10];
			char str_data[10];
			const char *str_type = NULL;
			U16 command;
			U8 data;
			snprintf(str_number, sizeof(str_number), "0x%08x", (U32)frame.mData1);

			if (ValidWord(frame.mData1, &command, &data)) {
				snprintf(str_cmd, sizeof(str_cmd), "0x%03X", command);
				snprintf(str_data, sizeof(str_data), "0x%02X", data);
				FormatDisplay(command, data, str_display, sizeof(str_display));
				str_type = FormatCommand(command);
			} else {
				snprintf(str_cmd, sizeof(str_cmd), "");
				snprintf(str_data, sizeof(str_data), "");
				snprintf(str_display, sizeof(str_display), "");
			}

			if (!str_type) {
				str_type = "";
			}
			file_stream << time_str << "," << str_number << "," << str_cmd << "," << str_type << "," << str_data << "," << str_display << std::endl;
		}

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void LogicDataAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	if (frame.mType == 1) {
		AddTabularText( "SYNC" );
	}
	if (frame.mType == 2) {
 		char output[16];
 		snprintf(output, sizeof(output), "0x%08X", (U32)frame.mData1);
		AddTabularText( output );
	}
#endif
}

void LogicDataAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void LogicDataAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}