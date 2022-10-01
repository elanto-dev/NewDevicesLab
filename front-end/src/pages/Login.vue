<template>
  <master-layout pageTitle="Login Form">
    <ion-card>
      <ion-card-header>
        <ion-card-title>Sign In</ion-card-title>
      </ion-card-header>
      <ion-card-content>
        <ion-item>
          <ion-label position="stacked">Email</ion-label>
          <ion-input v-model="userInfo.email"></ion-input>
        </ion-item>
        <ion-item>
          <ion-label position="stacked">Password</ion-label>
          <ion-input type="password" v-model="userInfo.password"></ion-input>
        </ion-item>
        <ion-button expand="block" @click="login()">Login</ion-button>
        <ion-button expand="block" @click="toRegister()">Go to register page</ion-button>
      </ion-card-content>
    </ion-card>
  </master-layout>
</template>
<script>
import {
  IonCard,
  IonCardHeader,
  IonCardTitle,
  IonCardContent,
  IonItem,
  IonLabel,
  IonInput,
} from "@ionic/vue";
import { mapGetters, mapActions } from "vuex";
export default {
  components: {
    IonCard,
    IonCardHeader,
    IonCardTitle,
    IonCardContent,
    IonItem,
    IonLabel,
    IonInput,
  },
  data() {
    return {
      userInfo: {
        email: "",
        password: "",
      },
    };
  },
  computed: {
    ...mapGetters("auth", {
      loginStatus: "getLoginStatus",
      authData: "getAuthData",
    }),
  },
  methods: {
    ...mapActions("auth", {
      loginUser: "loginUser",
    }),
    async login() {
      if (this.userInfo.email && this.userInfo.password) {
        await this.loginUser(this.userInfo);
        if(this.loginStatus == "success"){
          this.$router.push("/Main");
        }else{
          alert('failed to login')
        }
      }
    },
    async toRegister(){
      this.$router.push("/register");
    },
  },
};
</script>