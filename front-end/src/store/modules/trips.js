import axios from "axios";
import URL from "./URL"

const store = () => ({
    uploadStutus: "success",
});

const getters = {
    getUploadStatus(state) {
        return state.uploadStutus;
    }
};

const actions = {
    async getTrips({ commit }, token) {
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.get(URL + "Trips", header)
            if (response.status == 200 || response.status == 201) {
                commit("saveUploadStatus", "success");
                return response.data
            } else {
                console.log("Response status: ", response.status)
                return []
            }
        } catch (err) {
            console.log("Error: ", err)
            commit("saveUploadStatus", "failed");
            return []
        }
    },
    async getTripById({ commit }, { token, id }) {
        console.log("Shelves Token: ", token)
        console.log("Trip id:", id)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            console.log("header", header)
            console.log("Trip id:", id)
            const response = await axios.get(URL + "Trips/" + id, header)
            if (response.status == 200 || response.status == 201) {
                console.log(response.data)
                commit("saveUploadStatus", "success");
                return response.data
            } else {
                console.log("Response status: ", response.status)
                return []
            }
        } catch (err) {
            console.log("Error: ", err)
            commit("saveUploadStatus", "failed");
            return []
        }
    },
    async newTrip({commit}, {token, payload}){
        console.log("NewTripPost header: ", token)
        console.log("NewTripPost payload: ", payload)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.post(URL + "Trips", payload, header)
            console.log("NewTripPost response: ", response.status)
            if (response.status == 200 || response.status == 201) {
                commit("saveUploadStatus", "success");
                return true;
            } else {
                console.log("Response status: ", response.status);
                commit("saveUploadStatus", "success");
                return false;
            }
        } catch (err) {
            commit("saveUploadStatus", "failed");
            return false;
        }
    },
    async updateTrip({ commit }, {token, id, payload}) {
        console.log("NewTripPost header: ", payload)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        console.log("payload", payload, "token", token, "id", id)
        console.log("NewTripPost payload: ", payload.content)
        console.log("edit id", id)
        try {
            const response = await axios.put(URL + "Trips/" + id, payload, header)
            console.log(response, payload)
            if (response.status == 200 || response.status == 201) {
                commit("saveUploadStatus", "success");
                return true;
            } else {
                console.log("Response status: ", response.status)
                return false;
            }
        } catch (err) {
            commit("saveUploadStatus", "failed");
            return false;
        }
    },
    async deleteTrip({ commit }, {id, token}) {
        console.log("deleteTrip token: ", token)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.delete(URL + "Trips/" + id, header)
            if (response.status == 200 || response.status == 201) {
                commit("saveUploadStatus", "success");
                return true;
            } else {
                console.log("Response status: ", response.status)
                return false;
            }
        } catch (err) {
            commit("saveUploadStatus", "failed");
            return false;
        }
    },
};

const mutations = {
    saveUploadStatus(state, status) {
        state.uploadStutus = status;
    }
};

export default {
    namespaced: true,
    store,
    getters,
    actions,
    mutations,
};