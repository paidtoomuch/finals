#pragma once 

namespace render
{
	inline void( __fastcall* render_frame_o )( void*, void*, void* );

	inline hooks::c_vmt render_frame_vmt;

	class c_render
	{ 
	private:
		std::unique_ptr<settings::c_settings> m_setting	{};

	public:

		c_render( );

		bool setup( );

		void clean_context( );
		void release_objects( );

		void tick( );

	public:

		MARGINS m_margin { -1 };
		RECT m_rect { };
		MSG m_msg { };
		
		std::uint32_t m_pid { };
		std::string m_name { };

		HWND m_window_target { };
		HWND m_overlay { };

		int m_width { };
		int m_height { };

		int m_width_2 { };
		int m_height_2 { };
	};
} 

inline std::shared_ptr<render::c_render> m_render = std::make_shared<render::c_render>( );

namespace render
{
	inline auto render_frame_h( void* a1, void* a2, void* a3 )
	{
		stub::callstack( 
			m_stub.get( )->gadget_rbx,
			render_frame_vmt.get_original<decltype( render_frame_o )>( m_nom.get( )->data.render_index ),
			a1, 
			a2, 
			a3 
		);

		m_render.get( )->tick( );
	}
}