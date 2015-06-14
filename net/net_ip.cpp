#include "../class/class_main.h"
#include "../class/class_array.h"
#include "../util/util_main.h"
#include "net_ip.h"

std::string pull_global_ip_address(){
	std::string return_value;
	#ifdef __linux
		system("wget http://checkip.dyndns.org/ -o ip.file");
		std::ifstream in("ip.file");
		if(in.is_open() == false){
			printf("Could not parse the IP file\n");
		}else{
			char data[512];
			in.getline(data, 511);
			return_value = return_value.substr(return_value.find_first_of(":") + 2, return_value.find_first_of("</body>")-return_value.find_first_of(":")-2);
		}
	#else
		printf("Global IP pulling is a Linux only thing right now.\n");
	#endif
	if(return_value == ""){
		printf("Insert the GLOBAL IP address: ");
		char data[512];
		scanf("%s\n", data);
		return_value = data;
	}
	printf("Global IP address is %s\n", return_value.c_str());
	return return_value;
}

static std::string parse_ifconfig_line_for_ip_address(std::string tmp){
	std::string return_value;
	if(tmp.find_first_of("inet addr: ") != std::string::npos){
		const uint_ start_point = tmp.find_first_of("inet addr: ")+11;
		std::string tmp_2 = tmp;
		tmp_2.substr(start_point, 512);
		const uint_ size = tmp.find_first_of(" ")-1;
		return_value = tmp.substr(start_point, size);
	}
	return return_value;
}

std::string pull_local_ip_address(std::string local_ip){
	std::string return_value = "";
	#ifdef __linux
		system("ifconfig > ifconfig.file");
		std::ifstream in("ifconfig.file");
		if(in.is_open() == false){
			printf("Could not parse the IP file\n");
		}else{
			char data[512];
			while(in.getline(data, 512) && return_value == ""){
				std::string tmp = parse_ifconfig_line_for_ip_address(data);
				if(tmp != ""){
					if(local_ip == ""){
						if(tmp.find_first_of(local_ip.substr(0,local_ip.find_first_of("."))) != std::string::npos){
							return_value = tmp;
						}
					}else{
						return_value = tmp;
					}
				}
			}
			in.close();
		}
	#else
		printf("Local IP pulling is a Linux only thing right now.\n");
	#endif
	if(return_value == ""){
		char data_tmp[512];
		printf("Insert the LOCAL IP address: ");
		scanf("%s\n", data_tmp);
		return_value = data_tmp;
	}
	printf("Local IP address is %s\n", return_value.c_str());
	return return_value;
}

std::string net_gen_ip_address(std::string host_ip){
	std::string return_value;
	std::string first_segment = host_ip.substr(0, host_ip.find_first_of("."));
	printf("first_segment: %s\n", first_segment.c_str());
	if(first_segment == "192" || first_segment == "10"){
		return_value = pull_local_ip_address(host_ip);
	}else if(first_segment == "127"){
		return_value = "127.0.0.1";
	}else{
		return_value = pull_global_ip_address();
	}
	return return_value;
}

void net_ip_t::update_outbound_port(array_id_t new_net_id){
	delete outbound;
	outbound = new udp_socket_t(new_net_id);
}

void net_ip_t::update_inbound_port(array_id_t new_net_id){
	delete inbound;
	inbound = new udp_socket_t(new_net_id);
}

net_ip_t::net_ip_t() : array(this, "net_ip_t", false){
	array.reset_values();
}

void net_ip_t::set_inbound_info(array_id_t inbound_info_id){
	if(inbound == nullptr){
		delete inbound;
		inbound = nullptr;
	}
	inbound = new udp_socket_t(inbound_info_id);
	net_ip_connection_info_t *inbound_info = (net_ip_connection_info_t*)find_pointer(inbound_info_id);
	throw_if_nullptr(inbound_info);
	printf_("STATUS: net_ip_t::init: inbound ip: " + inbound_info->ip + "\tinbound port: " + std::to_string(inbound_info->port) + "\n", PRINTF_STATUS);
}

int_ net_ip_t::init(){
	SDLNet_Init();
	net_ip_connection_info_t *outbound_info = new net_ip_connection_info_t;
	outbound_info->port = 0;
	outbound_info->ip = "127.0.0.1";
	outbound = new udp_socket_t(outbound_info->array.id);
	printf_("STATUS: net_ip_t::init: outbound ip: " + outbound_info->ip + "\toutbound port: " + std::to_string(outbound_info->port) + "\n", PRINTF_STATUS);
	outbound_packet = SDLNet_AllocPacket(512);
	inbound_packet = SDLNet_AllocPacket(512);
	return 0;
}

std::string net_ip_t::read(std::string search){
	std::string return_value;
	std::vector<array_id_t> read_buffer = all_ids_of_type("net_ip_read_buffer_t");
	const uint_ read_buffer_size = read_buffer.size();
	for(uint_ i = 0;i < read_buffer_size;i++){
		try{
			net_ip_read_buffer_t* ptr = (net_ip_read_buffer_t*)find_pointer(read_buffer[i]);
			throw_if_nullptr(ptr);
			if(ptr->finished() && (search == "" || ptr->gen_string().find_first_of(search) != std::string::npos)){
				return_value = ptr->gen_string();
				delete ptr;
				break;
			}
		}catch(const std::logic_error &e){}
	}
	return return_value;
}

void net_ip_t::write(std::string data, int_ b, uint_ packet_id){
	new net_ip_write_buffer_t(data, b, packet_id);
}

std::string net_ip_t::receive_now(){
	inbound_packet->data[0] = '\0';
	inbound_packet->len = 0;
	SDLNet_UDP_Recv(inbound->get_socket(),inbound_packet);
	if(inbound_packet->len != 0){
		positive_to_negative_trick(&inbound_packet->data, inbound_packet->len);
	}
	return (char*)inbound_packet->data;
	//return reinterpret_cast<std::string>(inbound_packet->data);
}

int_ net_ip_t::send_now(net_ip_write_buffer_t *data){
	int_ return_value = 0;
	data->array.data_lock.lock();
	if(outbound == NULL){
		printf_("ERROR: Cannot use outbound port. Check to see if you have proper permissions to use raw sockets\n", PRINTF_ERROR);
		return_value = -1;
		return return_value;
	}
	uint_ position = 0;
	net_ip_connection_info_t *tmp_conn = (net_ip_connection_info_t*)find_pointer(data->connection_info_id);
	if(tmp_conn != nullptr){
		std::vector<std::string> raw_packets;
		std::string data_prefix = wrap(NET_PACKET_ID_START, std::to_string(data->packet_id), NET_PACKET_ID_END);
		if(data->data.size() > NET_MTU-NET_MTU_OVERHEAD){
			while(data->data != ""){
				uint_ chunk = NET_MTU-NET_MTU_OVERHEAD;
				std::string pos_prefix = wrap(NET_PACKET_POS_START, std::to_string(position), NET_PACKET_POS_END);
				std::string tmp_string = data_prefix + pos_prefix + data->data.substr(0,chunk);
				raw_packets.push_back(tmp_string);
				if(chunk > data->data.size()){
					break;
				}
				data->data = data->data.substr(chunk, data->data.size());
				position++;
			}
		}else{
			printf_("SPAM: The data to send is small enough to put into one packet\n", PRINTF_SPAM);
			std::string tmp;
			tmp = wrap(NET_PACKET_ID_START, std::to_string(data->packet_id), NET_PACKET_ID_END);
			tmp += wrap(NET_PACKET_POS_START, "0", NET_PACKET_POS_END);
			tmp += data->data;
			raw_packets.push_back(tmp);
		}
		const uint_ raw_packets_size = raw_packets.size();
		printf_("SPAM: net_ip_t::send_now: raw_packets_size: " + std::to_string(raw_packets_size) + "\n", PRINTF_SPAM);
		raw_packets[0] = NET_PACKET_START + raw_packets[0];
		raw_packets[raw_packets_size-1] += NET_PACKET_END;
		IPaddress IP;
		SDLNet_ResolveHost(&IP, tmp_conn->ip.c_str(), (unsigned short int)tmp_conn->port);
		outbound_packet->address.host = IP.host;
		outbound_packet->address.port = IP.port;
		uint_ total_byte_size = 0;
		const uint_ max_total_sent_byte = 8*KILOBYTE_TO_BYTE;
		for(uint_ i = 0;i < raw_packets_size;i++){
			negative_to_positive_trick(&raw_packets[i]);
			unsigned char* outbound_data = (unsigned char *)raw_packets[i].c_str();
			outbound_packet->len = (int)(raw_packets[i].size()+1);
			outbound_packet->data = outbound_data;
			if(SDLNet_UDP_Send(outbound->get_socket(), -1, outbound_packet) > 0){
				printf_("SPAM: send_now: HOLY MOLEY! SOMETHING SENT\n", PRINTF_SPAM);
			}
			total_byte_size += outbound_packet->len;
			if(unlikely(total_byte_size > max_total_sent_byte)){
				// no packet loss if the client is sending data to itself
				total_byte_size = 0;
				loop_receive();
			}
		}
		raw_packets.clear();
	}else{
		printf_("WARNING: net_ip_t::send_now: The connection ID (" + std::to_string(data->connection_info_id) + ") does not match up with anything here. CANNOT SEND THE DATA\n", PRINTF_STATUS);
	}
	data->array.data_lock.unlock();
	return return_value;
}

void net_ip_t::loop_send(){
	const std::vector<array_id_t> write_buffer = all_ids_of_type("net_ip_write_buffer_t");
	const uint_ write_buffer_size = write_buffer.size();
	total_sent_bytes = 0;
	if(write_buffer_size != 0){
		printf_("\tSPAM: net_ip_t::loop_send: write_buffer_size: " + std::to_string(write_buffer_size) + "\n", PRINTF_SPAM);
	}
	for(uint_ i = 0;i < write_buffer_size;i++){
		try{
			net_ip_write_buffer_t *ptr = (net_ip_write_buffer_t*)find_pointer(write_buffer[i]);
			throw_if_nullptr(ptr);
			send_now(ptr);
			delete ptr;
			ptr = nullptr;
		}catch(const std::logic_error &e){}
	}
}

bool net_ip_t::receive_check_read_array(std::string a, uint_ packet_id){
	std::vector<array_id_t> read_buffer = all_ids_of_type("net_ip_read_buffer_t");
	const uint_ read_buffer_size = read_buffer.size();
	bool return_value = false;
	for(uint_ i = 0;i < read_buffer_size;i++){
		try{
			net_ip_read_buffer_t *tmp = (net_ip_read_buffer_t*)find_pointer(read_buffer[i]);
			throw_if_nullptr(tmp);
			if(tmp->packet_id == packet_id){
				return_value = true;
				tmp->parse_packet_segment(a);
				break;
			}
		}catch(const std::logic_error &e){}
	}
	if(return_value == false){
		(new net_ip_read_buffer_t((int)packet_id))->parse_packet_segment(a);
	}
	return return_value;
}

void net_ip_t::loop_receive(){
	std::string inbound_packet_string;
	int_ curr_item = 0, max_item = 512;
	while((inbound_packet_string = receive_now()) != "" && curr_item != max_item){
		const uint_ start = inbound_packet_string.find_first_of(NET_PACKET_ID_START);
		const uint_ end = inbound_packet_string.find_first_of(NET_PACKET_ID_END);
		const std::string tmp_packet_id = inbound_packet_string.substr(start+1,end-start-1);
		const uint_ packet_id = atoi(tmp_packet_id.c_str());
		receive_check_read_array(inbound_packet_string, packet_id);
		curr_item++;
	}
}

void net_ip_t::loop(){
	loop_send();
	loop_receive();
}

void net_ip_t::close(){
	delete inbound;
	delete outbound;
	SDLNet_FreePacket(inbound_packet);
	outbound_packet->data = NULL;
	SDLNet_FreePacket(outbound_packet);
	SDLNet_Quit();
}

static net_packet_id net_packet_id_ = 0;

net_ip_write_buffer_t::net_ip_write_buffer_t(std::string a, net_packet_id b, int_ c) : array(this, "net_ip_write_buffer_t", ARRAY_SETTING_IMMUNITY){
	array.data_lock.lock();
	data = a;
	packet_id = ++net_packet_id_;
	connection_info_id = c;
	sent = false;
	array.data_lock.unlock();
}

std::vector<std::string> net_ip_write_buffer_t::gen_string_vector(){
	array.data_lock.lock();
	std::vector<std::string> return_value;
	std::string data_tmp = data;
	while(data_tmp.size() > 0){
		return_value.push_back(data_tmp.substr(0, NET_MTU-NET_MTU_OVERHEAD));
		data_tmp = data_tmp.substr(NET_MTU-NET_MTU_OVERHEAD, data_tmp.size());
	}
	array.data_lock.unlock();
	return return_value;
}


net_ip_read_buffer_t::net_ip_read_buffer_t(net_packet_id id) : array(this, "net_ip_read_buffer_t", ARRAY_SETTING_IMMUNITY){
	array.data_lock.lock();
	packet_id = id;
	array.data_lock.unlock();
}

net_ip_read_buffer_t::~net_ip_read_buffer_t(){}

void net_ip_read_buffer_t::parse_packet_segment(std::string segment){
	array.data_lock.lock();
	const uint_ start = segment.find_first_of(NET_PACKET_POS_START);
	const uint_ end = segment.find_first_of(NET_PACKET_POS_END);
	const std::string pos_str = segment.substr(start+1, end-start-1);
	const uint_ pos = atoi(pos_str.c_str());
	printf_("SPAM: net_ip_read_buffer_t::parse_packet_segment: pos: " + std::to_string(pos) + "\n", PRINTF_SPAM);
	data_vector.insert(data_vector.begin()+pos, segment);
	array.data_lock.unlock();
}

bool net_ip_read_buffer_t::finished(){
	array.data_lock.lock();
	const uint_ data_vector_size = data_vector.size();
	bool return_value = false;
	for(uint_ i = 0;i < data_vector_size;i++){
		if(data_vector[i] == ""){
			return_value = false;
			break;
		}
		if(data_vector[i].find_first_of(NET_PACKET_END) != std::string::npos){
			return_value = true;
			break;
		}
	}
	array.data_lock.unlock();
	return return_value;
}

std::string net_ip_read_buffer_t::gen_string(){
	array.data_lock.lock();
	std::string return_value;
	const uint_ data_vector_size = data_vector.size();
	for(uint_ i = 0;i < data_vector_size;i++){
		std::string tmp_value = data_vector[i];
		tmp_value.erase(tmp_value.find_first_of(NET_PACKET_ID_START), tmp_value.find_first_of(NET_PACKET_ID_END)-tmp_value.find_first_of(NET_PACKET_ID_START)+1);
		tmp_value.erase(tmp_value.find_first_of(NET_PACKET_POS_START), tmp_value.find_first_of(NET_PACKET_POS_END)-tmp_value.find_first_of(NET_PACKET_POS_START)+1);
		const uint_ packet_start = tmp_value.find_first_of(NET_PACKET_START);
		uint_  packet_end = tmp_value.find_first_of(NET_PACKET_END);
		if(packet_start != std::string::npos){
			tmp_value.erase(tmp_value.begin()+packet_start);
			if(packet_end != std::string::npos){
				packet_end -= 1;
			}
		}
		if(packet_end != std::string::npos){
			tmp_value.erase(tmp_value.begin()+packet_end);
		}
		return_value += tmp_value;
	}
	array.data_lock.unlock();
	return return_value;
}

udp_socket_t::udp_socket_t(array_id_t tmp_connection_info_id){
	connection_info_id = tmp_connection_info_id;
	net_ip_connection_info_t *tmp = (net_ip_connection_info_t*)find_pointer(connection_info_id);
	socket = nullptr;
	if(tmp == nullptr){
		printf_("ERROR: The net_ip_connection_info_t that this socket has to use does not exist (ID: " + std::to_string(tmp_connection_info_id) + ")\n", PRINTF_ERROR);
	}else{
		socket = SDLNet_UDP_Open(tmp->port);
		if(socket == nullptr){
			printf_("VITAL: Socket will not open (port: " + std::to_string(tmp->port) + ")\n", PRINTF_VITAL);
			assert(false);
		}else{
			printf_("STATUS: udp_socket_t::udp_socket_t: loaded socket correctly. ready for data transfers\n", PRINTF_STATUS);
		}
	}
}

udp_socket_t::~udp_socket_t(){
	if(socket != nullptr){
		SDLNet_UDP_Close(socket);
	}
}

UDPsocket udp_socket_t::get_socket(){
	if(socket == nullptr){
		printf_("VITAL: Socket is null (net_ip_connection_info_t ID (striped): " + std::to_string(strip_id(connection_info_id)) + "\n", PRINTF_VITAL);
	}
	return socket;
}
