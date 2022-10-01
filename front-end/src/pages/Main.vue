<template>
  <master-layout pageTitle="NDL Project app">
    <ion-card>
      <ion-card-header>
        <ion-card-title>Shelves</ion-card-title>
      </ion-card-header>
      <ion-card-content>
        <ion-item  v-for="shelf in shelves" :key="shelf.id">
          <ion-label>{{shelf.identifier}}</ion-label>
          <ion-label>{{shelf.location}}</ion-label>
          <ion-label>{{shelf.mainShelfId}}</ion-label>
          <ion-button expand="block" @click="editShelf(shelf.id)">Edit</ion-button>
          <ion-button expand="block" @click="removeShelf(shelf.id, shelf.identifier)" color="danger">Delete</ion-button>
        </ion-item>
        <ion-button expand="block" @click="addShelf()">Add shelf</ion-button>
      </ion-card-content>
    </ion-card>
    <ion-card>
      <ion-card-header>
        <ion-card-title>Trips</ion-card-title>
      </ion-card-header>
      <ion-card-content>
        <ion-item v-for="trip in trips" :key="trip.id">
          <ion-label>{{trip.location}}</ion-label>
          <ion-button expand="block" @click="editTrip(trip.id)">Edit</ion-button>
          <ion-button expand="block" @click="removeTrip(trip.id, trip.location, trip.startDateTime)" color="danger">Delete</ion-button>
        </ion-item>
        <ion-button expand="block" @click="addTrip()">Add trip</ion-button>
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
  IonLabel
} from "@ionic/vue";
import { mapGetters, mapActions } from "vuex";
export default {
  components: {
    IonCard,
    IonCardHeader,
    IonCardTitle,
    IonCardContent,
    IonItem,
    IonLabel
  },
  data () {
    return {
       userName: "",
       shelves: [],
       trips: []
    }
  },
  computed: {
    ...mapGetters("auth", {
        authData: "getUser",
      }
    )
  },
  async created () {
    await this.fetchData()
  },
  watch: {
    // call again the method if the route changes
    '$route': 'fetchData'
  },
  methods: {
    ...mapActions("shelves", [
        "getShelves",
        "deleteShelf",
    ]),
    ...mapActions("trips", [
      "getTrips",
      "deleteTrip"
    ]),
    async fetchData () {
        console.log("Main.vue Token", this.authData.token)
        this.userName = "Elina"
        this.shelves = (await this.getShelves(this.authData.token))
        this.trips = (await this.getTrips(this.authData.token))
        console.log(this.trips)
    },
    async editShelf(editId){
      this.$router.push({ path: '/AddEditShelf/' + editId });
    },
    async addShelf(){
      if (this.shelves.length < 5){
        this.$router.push("/AddEditShelf/0");
      }
      else{
        alert('limit of 5 shelves has been reached, please delete a shelf before adding a new one.')
      }
    },
    async removeShelf(id, identifier){
      if(confirm("Are you sure you want to delete shelf (identifier: " + identifier + ")?")){
        console.log("AddShelf token:", this.authData.token);
        var token = this.authData.token;
        var success = false;
        console.log("deleteShelf id: ", id)
        success = (await this.deleteShelf({id, token}))
        if(success){
          await this.fetchData()
        }else{
          alert('failed to remove shelf')
        }
      }
    },
    async editTrip(editId){
      this.$router.push("/AddEditTrip/" + editId);
    },
    async addTrip(){
      this.$router.push("/AddEditTrip/0");
    },
    async removeTrip(id, location, startDate){
      if(confirm("Are you sure you want to delete trip to " + location + " on " + startDate   + "?")){
        console.log("DelTrip token:", this.authData.token);
        var token = this.authData.token;
        var success = false;
        console.log("deleteShelf id: ", id)
        success = (await this.deleteTrip({id, token}))
        if(success){
          await this.fetchData()
        }else{
          alert('failed to remove shelf')
        }
      }
    },
  },
};
</script>
<style scoped>
#container {
  text-align: center;
  position: absolute;
  left: 0;
  right: 0;
  top: 50%;
  transform: translateY(-50%);
}

#container strong {
  font-size: 20px;
  line-height: 26px;
}

#container p {
  font-size: 16px;
  line-height: 22px;
  color: #8c8c8c;
  margin: 0;
}

#container a {
  text-decoration: none;
}
</style>