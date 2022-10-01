<template>
  <master-layout pageTitle="Add shelf">
    <ion-card>
      <ion-card-header>
        <ion-card-title>Add new shelf</ion-card-title>        
      </ion-card-header>
      <ion-card-content>
        <ion-item>
          <ion-label position="stacked">Shelf Identifier</ion-label>
          <ion-input v-model="shelf.identifier"></ion-input>
        </ion-item>
        <ion-item>
          <ion-label>Main Shelf</ion-label>
          <ion-checkbox v-model="shelf.main"></ion-checkbox>
        </ion-item>
        <ion-item>
          <ion-label position="stacked">Location</ion-label>
          <ion-input v-model="shelf.location"></ion-input>
        </ion-item>
        <ion-item>
          <ion-label position="stacked">Main Shelf Identifier</ion-label>
          <ion-select placeholder="Select One" v-model="shelf.mainShelfID"> 
            <ion-select-option v-for="selectShelf in shelves" :value="selectShelf.id" :key="selectShelf.id">{{ selectShelf.identifier }} - {{ selectShelf.location }}</ion-select-option>
          </ion-select>
        </ion-item>
        <ion-button expand="block" @click="addShelf()">Save</ion-button>
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
  IonButton,
  IonCheckbox,
  IonInput,
  IonSelect,
  IonSelectOption
} from "@ionic/vue";
import { mapGetters, mapActions} from "vuex"; 
export default {
  components: {
    IonCard,
    IonCardHeader,
    IonCardTitle,
    IonCardContent,
    IonItem,
    IonLabel,
    IonButton,
    IonCheckbox,
    IonInput,
    IonSelect,
    IonSelectOption
  },
  data() {
    return {
      shelves: [],
      shelf: { 
        id: 0,
        identifier: "",
        main: true,
        location: "",
        mainShelfID: 0
      },
      mainShelf: {
        id: 0,
        identifier: "",
        main: true,
        location: "",
      },
      shelfMainId: ""
    };
  },
  computed: {
    ...mapGetters("auth", {
      authData: "getUser",
    }),
    ...mapGetters("shelves", {
      uploadStatus: "getUploadStatus",
    })
  },
  async Created () {
    await this.fetchData()
  },
  async updated () {
    await this.fetchData()
  },
  methods:{
    ...mapActions("shelves", {
      newShelf: "newShelf",
      updateShelf: "updateShelf",
      getShelfById: "getShelfById",
      getShelves: "getShelves",
    }),
    async fetchData () { //fetch needed things like tokens and other shelf info
      console.log("AddShelf id:", this.shelf.id);
      var token = this.authData.token;
      console.log("AddShelf Token", token)
      var path = window.location.pathname;
      var segments = path.split("/");
      var id = segments[2];
      if(id != undefined && id > 0){
        var fetchShelf = (await this.getShelfById({ token, id}));
        this.shelf.id = fetchShelf.id;
        this.shelf.identifier = fetchShelf.identifier;
        this.shelf.main = fetchShelf.main;
        this.shelf.location = fetchShelf.location;
        if (fetchShelf.mainShelfId != undefined){
          this.shelf.mainShelfID = fetchShelf.mainShelfId;
          console.log("main id", this.shelf.mainShelfID)
        }
        console.log("AddShelf id:", this.shelf.id);
      }
      this.shelves = (await this.getShelves(token));
      console.log("shelves", this.shelves)
    },
    async addShelf(){
      var token = this.authData.token;
      console.log("AddShelf content:", this.shelf);
      if (this.shelf.identifier && this.shelf.location){
        var payload;
        if (this.shelf.main == true){ //prepare payload if added shelf is main
          this.mainShelf.identifier = this.shelf.identifier;
          this.mainShelf.id = this.shelf.id;
          this.mainShelf.main = this.shelf.main;
          this.mainShelf.location = this.shelf.location;
          payload= this.mainShelf;
        }
        else{
          payload = this.shelf;
          if (!payload.mainShelfID){return;}
          payload.mainShelfID = parseInt(payload.mainShelfID)
        }
        var success = false;
        var id = payload.id;
        console.log("addShelf id: ", payload.id)
        if(id == 0){  //post if it is a new shelf
          success = (await this.newShelf({token, payload}))
        }
        if(id > 0){ //put if it is a edit of an existing shelf
          success = (await this.updateShelf({token, id, payload}))
        }
        if(success){
          this.$router.push("/");
        }else{
          alert('failed to add shelf')
        }
      }
    },
    async onChangeMain(val){
      this.shelf.content.main = val.detail.checked;
    },
    shelfChange(val) {
      this.shelf.mainShelfID = val.detail.value;
    }
  }
};
</script>