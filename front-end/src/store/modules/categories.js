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
    async getCategories({ commit }, token) {
        let header = {
            headers: {
                Authorization: 'Bearer ' + token
            }
        }
        try {
            const response = await axios.get(URL + "Categories", header)
            console.log("getCategories response: ", response.status)
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
    }
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