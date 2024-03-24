#pragma once

#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((BYTE)(GetBits(x[0]) << 4 | GetBits(x[1])))

namespace pattern
{
	static auto search( uintptr_t StartAddress, uintptr_t EndAddress, const char* Pattern ) -> PBYTE
	{
		PBYTE ModuleStart = ( PBYTE ) StartAddress;
		if ( !ModuleStart )
			return nullptr;

		PBYTE ModuleEnd = ( PBYTE ) EndAddress;
		if ( !ModuleEnd )
			return nullptr;

		PBYTE FirstMatch = nullptr;
		const char* CurPatt = Pattern;
		for ( ; ModuleStart < ModuleEnd; ++ModuleStart )
		{
			bool SkipByte = ( *CurPatt == '\?' );
			if ( SkipByte || *ModuleStart == GetByte( CurPatt ) )
			{
				if ( !FirstMatch )
					FirstMatch = ModuleStart;
				SkipByte ? CurPatt += 2 : CurPatt += 3;
				if ( CurPatt[-1] == 0 )
				{
					return FirstMatch;
				}
			}
			else if ( FirstMatch )
			{
				ModuleStart = FirstMatch;
				FirstMatch = nullptr;
				CurPatt = Pattern;
			}
		}
		return nullptr;
	}

	static auto find( const char* pattern, uintptr_t mod ) -> std::uintptr_t
	{
		const auto ntdll = reinterpret_cast< const unsigned char* >( mod );
		const auto dos = reinterpret_cast< const IMAGE_DOS_HEADER* >( ntdll );
		const auto nt = reinterpret_cast< const IMAGE_NT_HEADERS* >( ntdll + dos->e_lfanew );
		const auto sections = IMAGE_FIRST_SECTION( nt );
		const auto num_sections = nt->FileHeader.NumberOfSections;

		constexpr char section_name[5]{ '.', 't', 'e', 'x', 't' };

		const auto section = std::find_if( sections, sections + num_sections, [ & ]( const auto& s ) 
		{
				return std::equal( s.Name, s.Name + 5, section_name );
		} );

		if ( section != sections + num_sections ) 
		{
			const auto va = ntdll + section->VirtualAddress;

			MEMORY_BASIC_INFORMATION mbi;

			for ( auto addr = va; addr < va + section->Misc.VirtualSize; addr += mbi.RegionSize ) 
			{
				VirtualQuery( addr, &mbi, sizeof( mbi ) );

				if ( ( mbi.Protect & PAGE_READONLY ) == 0 && mbi.Protect != PAGE_NOACCESS ) 
				{
					auto temp = search( ( std::uintptr_t ) addr, ( std::uintptr_t ) addr + mbi.RegionSize, pattern );

					if ( temp ) 
					{ 
						return ( std::uintptr_t ) temp; 
					}
				}
			}
		}
	}
}