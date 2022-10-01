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
    async getShelves({ commit }, token) {
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.get(URL + "Shelves", header)
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
    async getShelfById({ commit }, { token, id }) {
        console.log("Shelves Token: ", token)
        console.log("Shelf id:", id)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            console.log("header", header)
            console.log("Shelf id:", id)
            const response = await axios.get(URL + "Shelves/id/" + id, header)
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
    async newShelf({ commit }, {token, payload}) {
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.post(URL + "Shelves", payload, header)
            console.log("NewShelfPost response: ", response.status)
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
    async updateShelf({ commit }, {token, id, payload}) {
        console.log("NewShelfPost header: ", payload)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.put(URL + "Shelves/" + id, payload, header)
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
    async deleteShelf({ commit }, {id, token}) {
        console.log("deleteShelf token: ", token)
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.delete(URL + "Shelves/" + id, header)
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