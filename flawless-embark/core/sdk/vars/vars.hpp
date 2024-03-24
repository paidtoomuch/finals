#ifndef VARS_HPP
#define VARS_HPP

namespace vars
{
	class c_vars {

	private:
		const std::uintptr_t find_offset( wchar_t* class_name, wchar_t* member_name );

	public:
		c_vars( ) = default;

		[[nodiscard]] const bool setup( );
	};
} inline std::shared_ptr<vars::c_vars> m_vars = std::make_shared<vars::c_vars>( );

#endif 