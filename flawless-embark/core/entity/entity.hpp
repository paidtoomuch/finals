#ifndef ENTITY_HPP
#define ENTITY_HPP

namespace entity
{
	class c_entity
	{
		std::mutex	m_mutex { };
		std::vector<std::uintptr_t>  m_players{ };

	public:
		std::vector<std::uintptr_t>  m_actors{ };
		//std::vector<fortnite::c_item>  m_loot{ };

		ALWAYS_INLINE c_entity( ) { m_setting.get( )->load( ); this->m_players.reserve( 110 );/* this->m_loots.reserve( 300 );*/ };
		ALWAYS_INLINE ~c_entity( ) = default;

		ALWAYS_INLINE std::uint32_t size() { return m_actors.size(); }
		//ALWAYS_INLINE std::uint32_t loot_size() { return m_loot.size(); }
		ALWAYS_INLINE void reset( ) { this->m_players.clear( );/* this->m_loots.clear( );*/ }

		void update( );
		void tick( );
	};

} inline auto m_entity = std::make_unique<entity::c_entity>( );
//} inline const auto m_entity = std::make_unique<entity::c_entity>( );

#endif // ENTITY_HPP