#include "net_ip.h"

net_ip_connection_data_t::net_ip_connection_data_t(){
	ip = "";
	port = 0;
	connection_type = NET_IP_CONNECTION_TYPE_UNDEFINED;
}

void net_ip_info_t::blank(){
	server_ip = (char*)"127.0.0.1";
	server_port = 0;
	client_port = 0;
	mtu = NET_IP_MTU;
}
void net_ip_t::blank(){
	ip_info.blank();
}

void net_ip_t::set_receive_port(unsigned short int a){
	SDLNet_UDP_Close(inbound);
	inbound = SDLNet_UDP_Open(a);
	ip_info.client_port = a;
}

int net_ip_t::init(int argc,char** argv){
	blank();
	for(int i = 0;i < argc;i++){
		if(strcmp(argv[i],"--net-ip-server-ip") == 0){
			ip_info.server_ip = (char*)argv[i+1];
		}else if(strcmp(argv[i],"--net-ip-server-port") == 0){
			ip_info.server_port = (int)atoi(argv[i+1]);
		}
	}
	SDLNet_Init();
	outbound = SDLNet_UDP_Open(0);
	inbound = SDLNet_UDP_Open(ip_info.client_port);
	outbound_packet = SDLNet_AllocPacket(ip_info.mtu);
	inbound_packet = SDLNet_AllocPacket(ip_info.mtu);
	return 0;
}

std::string net_ip_t::read(){
	const int inbound_size = read_buffer.size();
	for(int i = 0;i < inbound_size;i++){
		if(read_buffer[i] != ""){
			std::string return_value = read_buffer[i];
			read_buffer[i] = "";
			return return_value;
		}
		read_buffer.erase(read_buffer.begin()+i);
	}
	return (std::string)"";
}

int net_ip_t::write(std::string data, unsigned short int port, std::string ip){
	int return_value = 0;
	const int outbound_size = write_buffer.size();
	for(int i = 0;i < outbound_size;i++){
		if(write_buffer[i].data == ""){
			write_buffer[i].port = port;
			write_buffer[i].ip = ip;
			write_buffer[i].data = data;
			break;
		}
	}
	return return_value;
}

std::string net_ip_t::receive_now(){
	if(SDLNet_UDP_Recv(inbound,inbound_packet)){
		return static_cast<std::string>((char*)inbound_packet->data);
	}
	return "";
}

void net_ip_t::send_now(net_ip_write_buffer_t data){
	IPaddress IP;
	SDLNet_ResolveHost(&IP,const_cast<const char*>(data.ip.c_str()),data.port);
	outbound_packet->address.host = IP.host;
	outbound_packet->address.port = IP.port;
	outbound_packet->data = (unsigned char*)data.data.c_str();
	outbound_packet->len = (int)strlen((char*)data.data.c_str())+1;
	SDLNet_UDP_Send(outbound,-1,outbound_packet);
}

void net_ip_t::loop_send(){
	const int outbound_size = write_buffer.size();
	for(int i = 0;i < outbound_size;i++){
		if(write_buffer[i].data != ""){
			send_now(write_buffer[i]);
			write_buffer[i].data = "";
		}
	}
}

void net_ip_t::loop_receive(){
	while(SDLNet_UDP_Recv(inbound,inbound_packet)){
		printf("Got a live one\n"); // silly
		int i = 0;
		const int inbound_size = read_buffer.size();
		for(i = 0;i < inbound_size;i++){
			if(read_buffer[i] == ""){
				break;
			}
		}
		read_buffer[i] = static_cast<std::string>((char*)inbound_packet->data);
	}
}

void net_ip_t::loop(){
	loop_send();
	loop_receive();
}

void net_ip_t::close(){
	SDLNet_UDP_Close(inbound);
	SDLNet_UDP_Close(outbound);
	SDLNet_FreePacket(inbound_packet);
	SDLNet_FreePacket(outbound_packet);
	//SDLNet_Close();
}
