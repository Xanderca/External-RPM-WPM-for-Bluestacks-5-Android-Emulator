#include <impl/includes.hpp>

using namespace std;


u32 libil2cpp_base{  };

int main(){

		if (!bluestacks->setup("com.dts.freefireth"))
			return 1;

		libil2cpp_base = bluestacks->get_module_base("libil2cpp.so", 1);  /* change 2nd argument int
                                                                           if elf headers/offset read return null or is not on 1st mapping address*/

		if (!libil2cpp_base)return 1;

		cout << "libbase= " << hex << libil2cpp_base << "\n";



        auto base_game_facade = memory->read<u32>(libil2cpp_base + 0x9EC1C48);
        auto game_facade = memory->read<u32>(base_game_facade);
        auto static_game_facade = memory->read<u32>(game_facade + 0x5c);
        auto current_game = memory->read<u32>(static_game_facade);
        auto current_match = memory->read<u32>(current_game + 0x50);
        auto match_status = memory->read<u32>(current_match + 0x8c);
        auto local_player = memory->read<u32>(current_match + 0x94);

        std::cout << "baseGameFacade= " << std::hex << base_game_facade << "\n";
        std::cout << "gameFacade= " << std::hex << game_facade << "\n";
        std::cout << "staticGameFacade= " << std::hex << static_game_facade << "\n";
        std::cout << "currentGame= " << std::hex << current_game << "\n";
        std::cout << "currentMatch= " << std::hex << current_match << "\n";
        std::cout << "matchStatus: " << std::dec << match_status << " | localPlayer= " << std::hex << local_player << "\n";


		
















	return 0;
}
