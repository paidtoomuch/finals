#pragma once

#define contains_record( address, type, field ) ( ( type* )( ( char* )( address ) - ( std::uintptr_t )( & ( (type* ) 0 ) -> field ) ) )

namespace customs
{
	namespace structs
	{
		struct list_entry
		{
			struct list_entry* flink;
			struct list_entry* blink;
		};

		struct unicode_string
		{
			unsigned short length;
			unsigned short maximumlength;
			wchar_t* buffer;
		};

		struct peb_ldr_data
		{
			unsigned long length;
			unsigned long initialized;
			const char* sshandle;
			list_entry inloadordermodulelist;
			list_entry inmemoryordermodulelist;
			list_entry ininitializationordermodulelist;
		};

		struct peb
		{
			unsigned char   reserved1[2];
			unsigned char   beingdebugged;
			unsigned char   reserved2[1];
			const char* reserved3[2];
			peb_ldr_data* ldr;
		};

		struct ldr_data_table_entry
		{
			list_entry inloadordermodulelist;
			list_entry inmemoryorderlinks;
			list_entry ininitializationordermodulelist;
			void* dllbase;
			void* entrypoint;
			union {
				unsigned long sizeofimage;
				const char* _dummy;
			};
			unicode_string fulldllname;
			unicode_string basedllname;
		};

		struct image_dos_header
		{
			unsigned short e_magic;
			unsigned short e_cblp;
			unsigned short e_cp;
			unsigned short e_crlc;
			unsigned short e_cparhdr;
			unsigned short e_minalloc;
			unsigned short e_maxalloc;
			unsigned short e_ss;
			unsigned short e_sp;
			unsigned short e_csum;
			unsigned short e_ip;
			unsigned short e_cs;
			unsigned short e_lfarlc;
			unsigned short e_ovno;
			unsigned short e_res[4];
			unsigned short e_oemid;
			unsigned short e_oeminfo;
			unsigned short e_res2[10];
			long e_lfanew;
		};

		struct image_file_header
		{
			unsigned short machine;
			unsigned short numberofsections;
			unsigned long timedatestamp;
			unsigned long pointertosymboltable;
			unsigned long numberofsymbols;
			unsigned short sizeofoptionalheader;
			unsigned short characteristics;
		};

		struct image_export_directory
		{
			unsigned long characteristics;
			unsigned long timedatestamp;
			unsigned short majorversion;
			unsigned short minorversion;
			unsigned long name;
			unsigned long base;
			unsigned long numberoffunctions;
			unsigned long numberofnames;
			unsigned long addressoffunctions;
			unsigned long addressofnames;
			unsigned long addressofnameordinals;
		};

		struct image_data_directory
		{
			unsigned long virtualaddress;
			unsigned long size;
		};

		struct image_optional_header
		{
			unsigned short magic;
			unsigned char majorlinkerversion;
			unsigned char minorlinkerversion;
			unsigned long sizeofcode;
			unsigned long sizeofinitializeddata;
			unsigned long sizeofuninitializeddata;
			unsigned long addressofentrypoint;
			unsigned long baseofcode;
			unsigned long long imagebase;
			unsigned long sectionalignment;
			unsigned long filealignment;
			unsigned short majoroperatingsystemversion;
			unsigned short minoroperatingsystemversion;
			unsigned short majorimageversion;
			unsigned short minorimageversion;
			unsigned short majorsubsystemversion;
			unsigned short minorsubsystemversion;
			unsigned long win32versionvalue;
			unsigned long sizeofimage;
			unsigned long sizeofheaders;
			unsigned long checksum;
			unsigned short subsystem;
			unsigned short dllcharacteristics;
			unsigned long long sizeofstackreserve;
			unsigned long long sizeofstackcommit;
			unsigned long long sizeofheapreserve;
			unsigned long long sizeofheapcommit;
			unsigned long loaderflags;
			unsigned long numberofrvaandsizes;
			image_data_directory datadirectory[16];
		};

		struct image_nt_headers
		{
			unsigned long signature;
			image_file_header fileheader;
			image_optional_header optionalheader;
		};
	}

	static auto wcslen( const wchar_t* str ) -> int
	{
		int counter = 0;
		if ( !str )
			return 0;
		for ( ; *str != '\0'; ++str )
			++counter;
		return counter;
	}

	static auto wcsicmp_insensitive( wchar_t* cs, wchar_t* ct ) -> int
	{
		auto len = wcslen( cs );
		if ( len != wcslen( ct ) )
			return false;

		for ( size_t i = 0; i < len; i++ )
			if ( ( cs[i] | L' ' ) != ( ct[i] | L' ' ) )
				return false;

		return true;
	}

	static auto in_rect( double x, double y, double radius, double coord_x, double coord_y ) -> bool
	{
		return coord_x >= x - radius && coord_x <= x + radius &&
			coord_y >= y - radius && coord_y <= y + radius;
	}

	static auto in_circle( double x, double y, double radius, double coord_x, double coord_y ) -> bool
	{
		if ( in_rect( x, coord_y, radius, x, coord_y ) )
		{
			double dx = x - coord_x;
			double dy = y - coord_y;
			dx *= dx;
			dy *= dy;
			double distance_squared = dx + dy;
			double radius_squared = radius * radius;
			return distance_squared <= radius_squared;
		} return false;
	}

	static auto get_module( wchar_t* name ) -> std::uintptr_t
	{
		const structs::peb* peb = reinterpret_cast< structs::peb* >( __readgsqword( 0x60 ) );
		if ( !peb ) return std::uintptr_t( 0 );

		const structs::list_entry head = peb->ldr->inmemoryordermodulelist;

		for ( auto curr = head; curr.flink != &peb->ldr->inmemoryordermodulelist; curr = *curr.flink ) {
			structs::ldr_data_table_entry* mod = reinterpret_cast< structs::ldr_data_table_entry* >( contains_record( curr.flink, structs::ldr_data_table_entry, inmemoryorderlinks ) );

			if ( mod->basedllname.buffer )
				if ( wcsicmp_insensitive( mod->basedllname.buffer, name ) )
					return std::uintptr_t( mod->dllbase );
		}

		return std::uintptr_t( 0 );
	}
}
