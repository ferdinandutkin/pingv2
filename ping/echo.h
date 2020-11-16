#pragma once

#include "api_headers.h"
#include "ip.h"
#include <chrono>

class basic_handle {
protected:
    HANDLE handle;
public:
    operator HANDLE() { return handle; }
    operator bool() { return handle != INVALID_HANDLE_VALUE; }
};


class icmpfile : public basic_handle {
public:
    icmpfile() {
        handle = IcmpCreateFile();
        if (!handle) {
            throw std::exception("Invalid handle value");
        }
    }

    ~icmpfile() {
        IcmpCloseHandle(handle);
    }

    struct echo_reply {

        echo_reply(ICMP_ECHO_REPLY reply) : address{ reply.Address }, data_size{ reply.DataSize }, data{ static_cast<char*>(reply.Data) }
            , time{ std::chrono::milliseconds{ reply.RoundTripTime } }, TTL{ reply.Options.Ttl }, status{ reply.Status } {}

        uint32_t status;
        int TTL;
        std::string data;
        uint16_t data_size;
        ip_address address;
        std::chrono::milliseconds time;
    };


    echo_reply send_echo(ip_address ip, std::string payload, std::chrono::milliseconds timeout) const {


        DWORD payload_size = payload.length();
        DWORD reply_buf_size = sizeof(ICMP_ECHO_REPLY) + payload_size + 8;

        auto reply = std::make_unique<uint8_t[]>(reply_buf_size);

        if (not IcmpSendEcho(handle, ip,
            static_cast<void*>(payload.data()), payload_size, nullptr, reply.get(), reply_buf_size, timeout.count()))
            throw std::exception((std::string("echo error: ") + std::to_string(GetLastError())).c_str());


        return *reinterpret_cast<ICMP_ECHO_REPLY*>(reply.get());


    }
};


struct wsa {
    WSADATA wsa_data;
    int result;
    wsa() {
        result = WSAStartup(0x202, &wsa_data);
    }
    ~wsa() {
        WSACleanup();
    }
};


class host_entry {
    hostent* base;
public:
    host_entry(std::string hostname) {
        if ((base = gethostbyname(hostname.c_str())) == nullptr) {
            throw std::exception("gethostbyname failed");

        }
        for (size_t i{}; base->h_addr_list[i]; ++i) {
            ip_count++;
        }

    }


    size_t ip_count{};

    operator std::string() const { return base->h_name; }

    ip_address operator[] (size_t index) const {
        if ((index >= ip_count) or (index < 0))
            throw std::range_error("ip index out of range");

        return { (*reinterpret_cast<in_addr*>(base->h_addr_list[index])).S_un.S_addr };

    }

    friend std::ostream& operator << (std::ostream& os, const host_entry& he) {
        return os << he.operator std::string();
    }


};
