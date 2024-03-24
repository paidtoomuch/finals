#ifndef HOOKS_HPP
#define HOOKS_HPP

namespace hooks
{
	class c_vmt
	{
	public:
		auto clone( void* address )
		{
			this->address = uintptr_t( address );
			this->original_vtable = *( uintptr_t** ) this->address;
			this->size = int32_t( 0 );

			do this->size += 1;
			while ( *( uintptr_t* ) ( uintptr_t( this->original_vtable ) + ( this->size * 8 ) ) );
		}

		auto apply( void* hook_function, uint32_t index ) -> void
		{
			auto vfunction_original = ( void* ) this->original_vtable[index];

			uintptr_t* fake_vtable = new uintptr_t[this->size * int32_t( 8 )];

			for ( int32_t i = 0; i < this->size; i++ ) {
				if ( i == index ) continue;
				fake_vtable[i] = *( uintptr_t* ) ( uintptr_t( this->original_vtable ) + ( i * int32_t( 8 ) ) );
			} fake_vtable[index] = ( uintptr_t ) hook_function;

			*( uintptr_t** ) this->address = fake_vtable;
		}

		template <typename T>
		T get_original( uint32_t Index )
		{
			return ( T ) this->original_vtable[Index];
		}

	private:

		uintptr_t address;
		uintptr_t* original_vtable;
		int32_t size;

	};
}

#endif 