#include "net_ip.h"

int net_ip_t::init(int argc, char** argv, int tmp_conn_id){
	net_ip_connection_info_t *tmp_conn = find_net_ip_connection_info_pointer(tmp_conn_id);
	assert(tmp_conn != nullptr);
	connection_info_id = tmp_conn_id;
	SDLNet_Init();
	if(!(outbound = SDLNet_UDP_Open(0))){
		printf("Could not open the outbound port\n");
	}else{
		printf("Opened the outbound socket\n");
	}
	outbound_packet = SDLNet_AllocPacket(512);
	if(tmp_conn->port != 0){
		if(!(inbound = SDLNet_UDP_Open(tmp_conn->port))){
			printf("Could not open the inbound port\n");
		}else{
			printf("Opened the inbound socket\n");
		}
	}else{
		printf("The tmp_conn port is zero, this should not happen\n");
	}
	inbound_packet = SDLNet_AllocPacket(512);
	return 0;
}

std::string net_ip_t::read(std::string search){
	std::string return_value = "";
	const unsigned long int read_buffer_size = read_buffer.size();
	for(unsigned long int i = 0;i < read_buffer_size;i++){
		if(read_buffer[i].finished() && (search == "" || read_buffer[i].gen_string().find_first_of(search) != std::string::npos)){
			return_value = read_buffer[i].gen_string();
			read_buffer.erase(read_buffer.begin()+i);
			return return_value;
		}
	}
	return return_value;
}

void net_ip_t::write(std::string data, int b, unsigned long int packet_id){
	net_ip_write_buffer_t write_buffer_tmp(data, packet_id, b);
	write_buffer.push_back(write_buffer_tmp);
}

std::string net_ip_t::receive_now(){
	if(SDLNet_UDP_Recv(inbound,inbound_packet)){
		return (char*)inbound_packet->data;
	}
	return "";
}

int net_ip_t::send_now(net_ip_write_buffer_t *data){
	int return_value = 0;
	unsigned long int old_total_sent_bytes = total_sent_bytes;
	net_ip_connection_info_t *tmp_conn = find_net_ip_connection_info_pointer(data->connection_info_id);
	if(outbound == NULL){
		printf("Cannot use outbound port. Check to see if you have proper permissions to use raw sockets\n");
		return_value = -1;
		return return_value;
	}
	unsigned long int position = 0;
	if(tmp_conn != nullptr){
		//printf("Destination IP: %s\n", tmp_conn->ip.c_str());
		//printf("Destination Port: %d\n", tmp_conn->port);
		std::vector<std::string> raw_packets;
		std::string data_prefix = wrap(NET_PACKET_ID_START, std::to_string(data->packet_id), NET_PACKET_ID_END);
		if(data->data.size() > NET_MTU-NET_MTU_OVERHEAD){
			while(data->data != ""){
				unsigned long int chunk = NET_MTU-NET_MTU_OVERHEAD;
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
		const unsigned long int raw_packets_size = raw_packets.size();
		if(old_total_sent_bytes == 0){
			raw_packets[0] = NET_PACKET_START + raw_packets[0];
		}
		if(total_sent_bytes < 8*KILOBYTE_TO_BYTE){
			raw_packets[raw_packets_size-1] += NET_PACKET_END;
		}
		IPaddress IP;
		SDLNet_ResolveHost(&IP, tmp_conn->ip.c_str(), tmp_conn->port);
		outbound_packet->address.host = IP.host;
		outbound_packet->address.port = IP.port;
		unsigned long int total_byte_size = 0;
		for(unsigned long int i = 0;i < raw_packets_size;i++){
			unsigned char* outbound_data = (unsigned char *)raw_packets[i].c_str();
			outbound_packet->len = raw_packets[i].size()+1;
			outbound_packet->data = outbound_data;
			SDLNet_UDP_Send(outbound, -1, outbound_packet);
			total_byte_size += outbound_packet->len;
			if(unlikely(total_byte_size > 8*KILOBYTE_TO_BYTE)){
				total_byte_size = 0;
				loop_receive();
			}
		}
		raw_packets.clear();
	}else{
		printf("The connection ID (%d) does not match up with anything here\n", data->connection_info_id);
	}
	return return_value;
}

void net_ip_t::loop_send(){
	const unsigned long int write_buffer_size = write_buffer.size();
	total_sent_bytes = 0;
	for(unsigned long int i = 0;i < write_buffer_size;i++){
		send_now(&write_buffer[i]);
	}
	write_buffer.clear();
}

bool net_ip_t::receive_check_read_array(std::string a, unsigned long int packet_id){
	const unsigned long int read_buffer_size = read_buffer.size();
	for(unsigned long int i = 0;i < read_buffer_size;i++){
		if(read_buffer[i].packet_id == packet_id){
			read_buffer[i].parse_packet_segment(a);
			return true;
		}
	}
	net_ip_read_buffer_t tmp_read(packet_id);
	tmp_read.parse_packet_segment(a);
	read_buffer.push_back(tmp_read);
	return true;
}

void net_ip_t::loop_receive(){
	std::string inbound_packet_string;
	while((inbound_packet_string = receive_now()) != ""){ // speed this up DRASTICALLLY or get it out of a loop
		const unsigned long int start = inbound_packet_string.find_first_of(NET_PACKET_ID_START)+1;
		const unsigned long int end = inbound_packet_string.find_first_of(NET_PACKET_ID_END)-1;
		const std::string tmp_packet_id = inbound_packet_string.substr(start,end-start);
		const unsigned long int packet_id = atoi(tmp_packet_id.c_str());
		receive_check_read_array(inbound_packet_string, packet_id);
	}
}

void net_ip_t::loop(){
	loop_send();
	loop_receive();
}

void net_ip_t::close(){
	SDLNet_UDP_Close(inbound);
	SDLNet_FreePacket(inbound_packet);
	SDLNet_UDP_Close(outbound);
	outbound_packet->data = NULL;
	SDLNet_FreePacket(outbound_packet);
	SDLNet_Quit();
}

int net_ip_t::loop_receive_mt(){
	while(terminate == false) loop_receive();
	return 0;
}

int net_ip_t::loop_send_mt(){
	while(terminate == false) loop_send();
	return 0;
}

net_ip_write_buffer_t::net_ip_write_buffer_t(std::string a, unsigned long int b, int c){
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


net_ip_read_buffer_t::net_ip_read_buffer_t(int id){
	packet_id = id;
}

net_ip_read_buffer_t::~net_ip_read_buffer_t(){}

void net_ip_read_buffer_t::parse_packet_segment(std::string segment){
	const unsigned long int start = segment.find_first_of(NET_PACKET_POS_START);
	const unsigned long int end = segment.find_first_of(NET_PACKET_POS_END);
	const std::string pos_str = segment.substr(start+1, end-start-1);
	const unsigned long int pos = atoi(pos_str.c_str());
	data_vector.insert(data_vector.begin()+pos, segment);
}

bool net_ip_read_buffer_t::finished(){
	const unsigned long int data_vector_size = data_vector.size();
	bool return_value = false;
	for(unsigned long int i = 0;i < data_vector_size;i++){
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
	const unsigned long int data_vector_size = data_vector.size();
	for(unsigned long int i = 0;i < data_vector_size;i++){
		std::string tmp_value = data_vector[i];
		tmp_value.erase(tmp_value.find_first_of(NET_PACKET_ID_START), tmp_value.find_first_of(NET_PACKET_ID_END)-tmp_value.find_first_of(NET_PACKET_ID_START)+1);
		tmp_value.erase(tmp_value.find_first_of(NET_PACKET_POS_START), tmp_value.find_first_of(NET_PACKET_POS_END)-tmp_value.find_first_of(NET_PACKET_POS_START)+1);
		const unsigned long int packet_start = tmp_value.find_first_of(NET_PACKET_START);
		unsigned long int packet_end = tmp_value.find_first_of(NET_PACKET_END);
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
