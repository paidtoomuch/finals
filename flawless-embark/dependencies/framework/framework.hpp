#pragma once 
namespace framework {

	class c_framework {

	public:
		finals::u_canvas* current_canvas;

	public:

		c_framework( ) = default;

		[[nodiscard]] uemath::flinearcolor get_color( int a,int r,int g,int b );

		[[nodiscard]] auto setup( finals::u_canvas* new_canvas ) -> auto
		{
			current_canvas = new_canvas;
		}

		void text( const fstring content,
			uemath::fvector2d screen,
			uemath::flinearcolor color,
			bool center,
			bool outlined,
			double scale = 1 );

		void rect( uemath::fvector2d position, uemath::fvector2d size, uemath::flinearcolor color, float thickness );
		void filled_rect( uemath::fvector2d position, uemath::fvector2d size,uemath::flinearcolor color );
		void line( uemath::fvector2d x, uemath::fvector2d y,uemath::flinearcolor color, float thickness );
		void circle( uemath::fvector2d position, uemath::flinearcolor color, double radius, double segments );

	};

} inline std::shared_ptr<framework::c_framework> m_framework = std::make_shared<framework::c_framework>( );