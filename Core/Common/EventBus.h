// 
// Core: EventBus.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <Game/Api/nwapi_internal.hpp>
#include "Logger.h"

class NWAPI EventBus {
public:
    /**
     * \brief To register a function for future `call`
     * \param funcName The name of the function
     * \param func The pointer to the function
     * \sa REGISTER_AUTO
     * \note The difference between `registerFunc` and `subscribe` is that
     *       `registerFunc` will assume this is the only subscriber.
     *       You can use it like:
     *        \code{.cpp}
     *        int add(int a, int b) { return a+b; }
     *        registerFunc("add", add);
     *        // or
     *        REGISTER_AUTO(add);
     *        // since the identifier we used ("add") is same as the function name
     *        \endcode
     */
    template <typename T>
    void registerFunc(const std::string& funcName, T func) {
        auto& list = getSubscribers<T>(funcName);
        list.emplace_back(reinterpret_cast<FunctionPointer>(func));
        if (list.size() == 1)
            debugstream << "Function " << funcName << " with type " << typeid(T).name() << " (hash: " << typeid(T).
                hash_code() <<
                ") registered.";
        else
            warningstream << "Multiple(" << list.size() << ") functions with name"
                << funcName << " and type " << typeid(T).name() << " (hash: " << typeid(T).hash_code() <<
                ") registered.";
    }

    /**
    * \brief To subscribe a function to be called for future `publish`
    * \param funcName The name of the function
    * \param func The pointer to the function
    * \sa SUBSCRIBE_AUTO
    * \note The difference between `registerFunc` and `subscribe` is that
    *       `registerFunc` will assume this is the only subscriber.
    * \note See also registerFunc for the usage
    * \sa registerFunc
    */
    template <typename T>
    void subscribe(const std::string& funcName, T func) {
        getSubscribers<T>(funcName).emplace_back(reinterpret_cast<FunctionPointer>(func));
        debugstream << "Subscribed to " << funcName << " with " << typeid(T).name() << " (hash: " << typeid(T).
            hash_code() <<
            ")";
    }

    /**
    * \brief To call a function that is previously `registerFunc`ed
    * \tparam T the signature of the function
    * \param funcName The name of the function
    * \param args The arguments
    * \sa CALL_AUTO
    * \note The difference between `call` and `publish` is that `call`
    *       will assume that there is only one subscriber and has return value.
    *       You can use it like:
    *       \code{.cpp}
    *        int val = call<int(int, int)>("add", 1, 2);
    *        // or
    *        int add(int a, int b);
    *        CALL_AUTO(add, 1, 2);
    *       \endcode
    */
    template <typename T, typename... Args>
    auto call(const std::string& funcName, Args&&... args) {
        auto& list = getSubscribers<T>(funcName);
        if (list.size() == 0) {
            warningstream << "Failed to call function " << funcName
                << " with type " << typeid(T).name() << " (hash: " << typeid(T).hash_code() << "): "
                << (list.empty()
                        ? "No such function registered"
                        : "Multiple(" + std::to_string(list.size()) + ") functions registered.");
            throw std::runtime_error(funcName + " with type " + typeid(T).name()
                + " (hash: " + std::to_string(typeid(T).hash_code()) + ") does not exist");
        }
        return reinterpret_cast<T>(list[0])(std::forward<Args>(args)...);
    }

    /**
    * \brief To call a function that is previously `registerFunc`ed
    * \tparam T the signature of the function
    * \param funcName The name of the function
    * \param args The arguments
    * \sa PUBLISH_AUTO
    * \note The difference between `call` and `publish` is that `call`
    *       will assume that there is only one subscriber and has return value.
    *       You can use it like:
    *       \code{.cpp}
    *        publish<void(bool)>("onServerStarted", true);
    *        // or
    *        void onServerStarted(bool localServer);
    *        PUBLISH_AUTO(onServerStarted, true);
    *       \endcode
    */
    template <typename T, typename... Args>
    void publish(const std::string& funcName, Args&&... args) {
        auto& subscribers = getSubscribers<T>(funcName);
        for (auto& subscriber : subscribers)
            reinterpret_cast<T>(subscriber)(std::forward<Args>(args)...);
    }

private:
    using FunctionPointer = std::add_pointer_t<void()>;

    template <typename T>
    std::vector<FunctionPointer>& getSubscribers(const std::string& funcName) noexcept {
        return mSubscribers[std::to_string(typeid(T).hash_code()) + "!" + funcName];
    }

    std::unordered_map<std::string, std::vector<FunctionPointer>> mSubscribers;
};

extern NWCOREAPI EventBus eventBus;

/**
 * \brief Same as EventBus::registerFunc, except that it assumes the function
 *        it used in the source code is the same as the one you want to be registered.
 *        i.e. the function name will automatically be used as identifier in later `call`.
 * \param FUNC The function to be registered
 */
#define REGISTER_AUTO(FUNC) eventBus.registerFunc(#FUNC, FUNC)
/**
 * \brief Same as EventBus::call. It can be used when you have the declaration of the
 *        function available. Call it like `CALL_AUTO(funcDeclaration, arg1, arg2...)`
 * \param FUNC The function to be called
 */
#define CALL_AUTO(FUNC, ...) eventBus.call<decltype(FUNC)*>(#FUNC, __VA_ARGS__)
/**
 * \brief Same as EventBus::subscribe, except that it assumes the function
 *        it used in the source code is the same as the one you want to be subscribed.
 *        i.e. the function name will automatically be used as identifier in later `publish`.
 * \param FUNC The function to be subscribed
 */
#define SUBSCRIBE_AUTO(FUNC) eventBus.subscribe(#FUNC, FUNC)
/**
 * \brief Same as EventBus::publish. It can be used when you have the declaration of the
 *        function available. Call it like `PUBLISH_AUTO(funcDeclaration, arg1, arg2...)`
 * \param FUNC The function to be published
 */
#define PUBLISH_AUTO(FUNC, ...) eventBus.publish<decltype(FUNC)*>(#FUNC, __VA_ARGS__)
