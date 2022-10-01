<template>
  <master-layout pageTitle="Register Form">
    <ion-card>
      <ion-card-header>
        <ion-card-title>Sign Up</ion-card-title>
      </ion-card-header>
      <ion-card-content>
        <ion-item>
          <ion-label position="stacked">User Name</ion-label>
          <ion-input v-model="userInfo.UserName"></ion-input>
        </ion-item>
        <ion-item>
          <ion-label position="stacked">Email</ion-label>
          <ion-input type="email" v-model="userInfo.Email"></ion-input>
        </ion-item>
        <ion-item>
          <ion-label position="stacked">Password</ion-label>
          <ion-input type="password" v-model="userInfo.Password"></ion-input>
        </ion-item>
        <ion-button expand="full" @click="register()">Register</ion-button>
        <ion-button expand="full" @click="toLogin()">Go to login page</ion-button>
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
        UserName: "",
        Email: "",
        Password: "",
      },
    };
  },
  computed: {
    ...mapGetters("auth", {
      loginStatus: "getLoginStatus",
    }),
  },
  methods: {
    ...mapActions("auth", {
      registerUser: "registerUser",
    }),
    async register() {
      if (this.userInfo.UserName && this.userInfo.Email && this.userInfo.Password) {
        await this.registerUser(this.userInfo);
        if(this.loginStatus === "success"){
            this.$router.push("/Main");
        }else{
            alert('failed to Register')
        }
      }
    },
    async toLogin(){
      this.$router.push("/login");
    }
  },
};
</script>