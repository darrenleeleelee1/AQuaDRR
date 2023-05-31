#pragma once
#include <ctime>
#include <string>

struct Timer{
    std::clock_t st, mt;
    Timer(){
        this->st = std::clock();
    }
    void reset(std::clock_t &tmp){
        tmp = std::clock();
    }
    std::string getTime(){
        return std::to_string((std::clock() - this->st) / CLOCKS_PER_SEC) + "s";
    }
    std::string getAndReset(){
        std::clock_t tmp = this->st;
        this->reset(this->st);
        return std::to_string((std::clock() - tmp) / CLOCKS_PER_SEC) + "s";
    }
    void setShortTerm(){
        this->mt = std::clock();
    }
    std::string getShortTerm(){
        std::clock_t tmp = this->mt;
        this->reset(this->mt);
        return std::to_string((std::clock() - tmp) / CLOCKS_PER_SEC) + "s";
    }
};