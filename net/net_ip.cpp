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

net_ip_t::net_ip_t() : array(this, false){
}

int_ net_ip_t::init(int_ argc, char** argv, array_id_t tmp_conn_id){
	SDLNet_Init();
	net_ip_connection_info_t *tmp_conn_2 = new net_ip_connection_info_t;
	tmp_conn_2->port = 0;
	tmp_conn_2->ip = "127.0.0.1";
	outbound = new udp_socket_t(tmp_conn_2->array.id);
	inbound = new udp_socket_t(tmp_conn_id);
	outbound_packet = SDLNet_AllocPacket(512);
	inbound_packet = SDLNet_AllocPacket(512);
	return 0;
}

std::string net_ip_t::read(std::string search){
	read_buffer_lock.lock();
	std::string return_value = "";
	const uint_ read_buffer_size = read_buffer.size();
	for(uint_ i = 0;i < read_buffer_size;i++){
		if(read_buffer[i].finished() && (search == "" || read_buffer[i].gen_string().find_first_of(search) != std::string::npos)){
			return_value = read_buffer[i].gen_string();
			read_buffer.erase(read_buffer.begin()+i);
			break;
		}
	}
	read_buffer_lock.unlock();
	return return_value;
}

void net_ip_t::write(std::string data, int_ b, uint_ packet_id){
	net_ip_write_buffer_t write_buffer_tmp(data, b, packet_id);
	write_buffer_lock.lock();
	write_buffer.push_back(write_buffer_tmp);
	write_buffer_lock.unlock();
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
	net_ip_connection_info_t *tmp_conn = (net_ip_connection_info_t*)find_pointer(data->connection_info_id);
	if(outbound == NULL){
		printf("Cannot use outbound port. Check to see if you have proper permissions to use raw sockets\n");
		return_value = -1;
		return return_value;
	}
	uint_ position = 0;
	if(tmp_conn != nullptr){
		std::vector<std::string> raw_packets;
		std::string data_prefix = wrap(NET_PACKET_ID_START, std::to_string(data->packet_id), NET_PACKET_ID_END);
		if(data->data.size() > NET_MTU-NET_MTU_OVERHEAD){
			while(data->data != ""){
				uint_ chunk = NET_MTU-NET_MTU_OVERHEAD;
				std::string pos_prefix = wrap(NET_PACKET_POS_START, std::to_string( position), NET_PACKET_POS_END);
				std::string tmp_string = data_prefix + pos_prefix + data->data.substr(0,chunk);
				raw_packets.push_back(tmp_string);
				if(chunk > data->data.size()){
					break;
				}
				data->data = data->data.substr(chunk, data->data.size());
				position++;
			}
		}else{
			std::string tmp;
			tmp = wrap(NET_PACKET_ID_START, std::to_string(data->packet_id), NET_PACKET_ID_END);
			tmp += wrap(NET_PACKET_POS_START, "0", NET_PACKET_POS_END);
			tmp += data->data;
			raw_packets.push_back(tmp);
		}
		const uint_ raw_packets_size = raw_packets.size();
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
			SDLNet_UDP_Send(outbound->socket, -1, outbound_packet);
			total_byte_size += outbound_packet->len;
			if(unlikely(total_byte_size > max_total_sent_byte)){
				// no packet loss if the client is sending data to itself
				total_byte_size = 0;
				loop_receive();
			}
		}
		raw_packets.clear();
	}else{
		printf_("The connection ID (" + std::to_string(data->connection_info_id) + ") does not match up with anything here. CANNOT SEND THE DATA\n", PRINTF_STATUS);
	}
	return return_value;
}

void net_ip_t::loop_send(){
	const uint_ write_buffer_size = write_buffer.size();
	total_sent_bytes = 0;
	write_buffer_lock.lock();
	for(uint_ i = 0;i < write_buffer_size;i++){
		send_now(&write_buffer[i]);
	}
	write_buffer.clear();
	write_buffer_lock.unlock();
}

bool net_ip_t::receive_check_read_array(std::string a, uint_ packet_id){
	const uint_ read_buffer_size = read_buffer.size();
	read_buffer_lock.lock();
	bool return_value = false;
	for(uint_ i = 0;i < read_buffer_size;i++){
		if(read_buffer[i].packet_id == packet_id){
			return_value = true;
			read_buffer[i].parse_packet_segment(a);
			break;
		}
	}
	if(return_value == false){
		net_ip_read_buffer_t tmp_read((int)packet_id);
		tmp_read.parse_packet_segment(a);
		read_buffer.push_back(tmp_read);
	}
	read_buffer_lock.unlock();
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

net_ip_write_buffer_t::net_ip_write_buffer_t(std::string a, net_packet_id b, int_ c){
	data = a;
	packet_id = b;
	connection_info_id = c;
	sent = false;
}

std::vector<std::string> net_ip_write_buffer_t::gen_string_vector(){
	std::vector<std::string> return_value;
	std::string data_tmp = data;
	while(data_tmp.size() > 0){
		return_value.push_back(data_tmp.substr(0, NET_MTU-NET_MTU_OVERHEAD));
		data_tmp = data_tmp.substr(NET_MTU-NET_MTU_OVERHEAD, data_tmp.size());
	}
	return return_value;
}


net_ip_read_buffer_t::net_ip_read_buffer_t(net_packet_id id){
	packet_id = id;
}

net_ip_read_buffer_t::~net_ip_read_buffer_t(){}

void net_ip_read_buffer_t::parse_packet_segment(std::string segment){
	const uint_ start = segment.find_first_of(NET_PACKET_POS_START);
	const uint_ end = segment.find_first_of(NET_PACKET_POS_END);
	const std::string pos_str = segment.substr(start+1, end-start-1);
	const uint_ pos = atoi(pos_str.c_str());
	data_vector.insert(data_vector.begin()+pos, segment);
}

bool net_ip_read_buffer_t::finished(){
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
	return return_value;
}

std::string net_ip_read_buffer_t::gen_string(){
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
	return return_value;
}

udp_socket_t::udp_socket_t(array_id_t tmp_connection_info_id){
	connection_info_id = tmp_connection_info_id;
	net_ip_connection_info_t *tmp = (net_ip_connection_info_t*)find_pointer(connection_info_id);
	socket = nullptr;
	if(tmp == nullptr){
		printf_("The net_ip_connection_info_t that this socket has to use does not exist (ID: " + std::to_string(tmp_connection_info_id) + ")\n", PRINTF_ERROR);
	}else{
		socket = SDLNet_UDP_Open(tmp->port);
		if(unlikely(!socket)){
			printf_("Socket will not open (port: " + std::to_string(tmp->port) + ")\n", PRINTF_VITAL);
			assert(false);
		}
	}
}

udp_socket_t::~udp_socket_t(){
	if(likely(!socket)){
		SDLNet_UDP_Close(socket);
	}
}

UDPsocket udp_socket_t::get_socket(){
	if(socket == nullptr){
		printf_("ERROR: Socket is null (net_ip_connection_info_t ID (striped): " + std::to_string(strip_id(connection_info_id)) + "\n", PRINTF_VITAL);
	}
	return socket;
}
