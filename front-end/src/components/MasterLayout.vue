<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-buttons slot="primary">
          <ion-button @click="reload()" >
            Logout
          </ion-button>
          <ion-button @click="home()" >
            Home
          </ion-button>
        </ion-buttons>
        <ion-title>{{ pageTitle }}</ion-title>
      </ion-toolbar>
    </ion-header>
    <ion-content>
      <slot> </slot>
    </ion-content>
  </ion-page>
</template>
<script>
import {
  IonPage,
  IonHeader,
  IonContent,
  IonToolbar,
  IonTitle,
  IonButton,
  IonButtons, 
} from "@ionic/vue";

export default {
  components: {
    IonPage,
    IonHeader,
    IonContent,
    IonToolbar,
    IonTitle,
    IonButton, 
    IonButtons,
  },
  props: ["pageTitle"],
  methods:{
    async home(){
      this.$router.push("/");
    },
    async reload(){
      window.location.reload();
    }
  }
  
};
</script>
<style scoped>
ion-toolbar{
    --background: var(--ion-color-primary);
    --color: var(--ion-color-primary-contrast)
}
</style>