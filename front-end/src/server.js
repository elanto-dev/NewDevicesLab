const loginURL = "http://192.168.1.236:11226/api/Login"
const registerURL = "http://192.168.1.236:11226/api/Register"
const shelvesURL = "http://192.168.1.236:11226/api/Shelves"

const URLgetters = {
    getRegisterURL(){
        return registerURL;
    },
    getLoginURL(){
        return loginURL;
    },
    getShelvesURL(){
        return shelvesURL;
    },
}

export default{
    namespaced: true,
    URLgetters,
}