import { Plugins } from "@capacitor/core";
import jwt_decode from "jwt-decode";
import axios from "axios";
import URL from "./URL"

const userNameClaim = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims/name";
const { Storage } = Plugins;

const state = () => ({
    token: "",
    loginStatus: "success",
    userName: ""
});

const getters = {
    getLoginStatus(state) {
        return state.loginStatus;
    },
    getUserName(state) {
        return state.userName;
    },
    getAuthData(state) {
        return state.token;
    },
    getUser(state) {
        return state;
    },
};

const actions = {
    async loginUser({ commit }, payload) {
        console.log(URL);
        try {
            const response = await axios.post(URL + "Login", payload)
            if (response.status == 200 || response.status == 201) {
                Storage.set({
                    key: "token",
                    value: response.data.token,
                });
                commit("saveAuthToken", response.data);
                var decoded = jwt_decode(response.data.token);
                commit("saveUserName", decoded[userNameClaim]);
                commit("saveLoginStatus", "success");
                console.log("upload??", this.getUploadStatus)
            } else {
                commit("saveLoginStatus", "failed");
            }
        } catch (err) {
            commit("saveLoginStatus", "failed");
        }
    },
    async registerUser({ commit }, payload) {
        try {
            const response = await axios.post(URL + "Register", payload) 
            if (response.status == 200 || response.status == 201) {
                Storage.set({
                    key: "token",
                    value: response.data.token,
                });

                commit("saveAuthToken", response.data);
                commit("saveLoginStatus", "success");
            } else {
                commit("saveLoginStatus", "failed");
            }
        } catch (err) {
            console.log(err);
            commit("saveLoginStatus", "failed");
        }
    },
    async loadStorageTokens({ commit }) {
        const token = await Storage.get({ key: "token" });
        if (token) {
            const tokenData = { token: token.value };
            commit("saveAuthToken", tokenData);
        }
    },
    async saveTokensToStorage({ commit }, payload) {
        await Storage.set({
            key: "token",
            value: payload.token,
        });
        commit("saveTokenData", payload);
    }
};

const mutations = {
    saveAuthToken(state, payload) {
        state.token = payload.token;
    },
    saveLoginStatus(state, status) {
        state.loginStatus = status;
    },
    saveUserName(state, name) {
        state.userName = name;
    },
};

export default {
    namespaced: true,
    state,
    getters,
    actions,
    mutations,
};