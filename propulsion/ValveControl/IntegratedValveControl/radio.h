#ifndef __RADIO_H__
#define __RADIO_H__

const int packet_size = 4;
byte commands[3];

class SignalPacket{
  private:
  byte self_id;
  byte cur_trans_id;
  byte cur_packet_type;
  byte cur_sequence_code;
  byte* cur_recvd_packet;
  byte* cur_recvd_command;
  byte* cur_ack_command;
  int full_packet_size = packet_size+2;

  public: 
  SignalPacket(byte self_id): self_id{self_id} {};

  void set_self_id(byte cur_id){
    self_id = cur_id;
  }

  bool is_command()
  {
    return cur_packet_type == 1;
  }
  
  void refresh_cur_packet(byte* p_cur_packet)
  {
    cur_recvd_packet = p_cur_packet;
    cur_trans_id = cur_recvd_packet[0] >> 4;
    cur_packet_type = cur_recvd_packet[1];
    cur_sequence_code = cur_recvd_packet[2];
    if (is_command()){
      cur_recvd_command = cur_recvd_packet + 3;
      update_cur_global_command();
    }
  }

  void update_cur_global_command(){ // update the global variable command
    if (is_command()){
      for (int i=0; i<3; i++){
        commands[i] = cur_recvd_command[i];
      }
    }
  }
  
  void get_ack_command(byte* buf){ 
    buf[0] = cur_trans_id + (self_id<<4);
    buf[1] = 100;
    buf[2] = cur_sequence_code;
  }
};

class RfTr{
  private:
  byte self_id;
  SignalPacket cur_packet;
  char raw_data_packet[packet_size+2];
   
  public:
  RfTr(byte raw_self_id): cur_packet(raw_self_id), self_id{raw_self_id} {
  }
  
  byte check_id(byte * raw_cur_packet){
    byte recv_id = raw_cur_packet[0] & 0b1111;
    Serial.print("recv  id: ");
    Serial.println(recv_id);
    if(recv_id == self_id || recv_id == 15) {
      Serial.println("id match");
      return true;
    }  else {
      Serial.println("id mismatch");
      return false;
    }
  }

  void check_receive_packet(){
    if (get_packet_size()){
      receive_data();
    }
  }

  int get_packet_size()
  {
    int packet_size = LoRa.parsePacket();
    return packet_size;
  }
  
  void receive_data() {
    int i = 0;
    while(LoRa.available()){
      byte command = LoRa.read();
      raw_data_packet[i] = command;
      ++i;
    }
    Serial.println((byte) raw_data_packet[0], BIN);
    if (check_id(raw_data_packet)){
      Serial.print("Recieving data: {");
      for (int j = 0; j<i; ++j) {
        Serial.print((byte) raw_data_packet[j]);
        Serial.print(", ");
      }
      Serial.println("}");

      cur_packet.refresh_cur_packet(raw_data_packet);
      send_ack();
    }
  }

  void send_ack() {
    Serial.println("sending ack");
    byte buf[3];
    LoRa.beginPacket();
    cur_packet.get_ack_command(buf);
    LoRa.write(buf, 3);
    LoRa.endPacket();

  }
};

#endif
