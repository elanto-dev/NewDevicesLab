<template>
  <master-layout  pageTitle="Trips">
    <ion-card>
      <ion-card-content :fullscreen="true">
        <ion-list>
          <ion-item>
            <ion-label position="stacked">Location</ion-label>
            <ion-input v-model="trip.location"></ion-input>
          </ion-item>
          <ion-item>
            <ion-label position="stacked">Start Time</ion-label>
            <ion-datetime display-format="YYYY, MMM DD,  HH:mm" 
                          picker-format="YYYY, MMM DD HH:mm" 
                          min="2021" max="2030" 
                          value="2022-01-11T08:00Z"
                          @ionChange="onChangeStart"
                          ref="StartDateTime">>
            </ion-datetime>
          </ion-item>
          <ion-item>
            <ion-label position="stacked">End Time</ion-label>
            <ion-datetime display-format="YYYY, MMM DD,  HH:mm" 
                          picker-format="YYYY, MMM DD HH:mm" 
                          min="2022" max="2030" 
                          value="2022-01-11T09:00Z"
                          @ionChange="onChangeEnd"
                          ref="EndDateTime">
                          >
            </ion-datetime>
          </ion-item>
          <ion-item>
            <ion-label position="stacked">Reminder in seconds</ion-label>
            <ion-input v-on:keypress="numbersOnly" v-model="trip.reminderSeconds"></ion-input>
          </ion-item>
          <ion-item lines="none">
            <ion-label position="stacked">Shelves</ion-label>
          </ion-item>
          <ion-item lines="none" v-for="(shelf,Index) in shelves" :key="shelf.id" >  
            <ion-card>
              <ion-card-header>
                <ion-card-title>{{shelf.identifier}} - {{shelf.location}}</ion-card-title>
              </ion-card-header>
              <ion-card-content>
                <ion-grid>
                  <ion-row>
                    <ion-col>
                      <ion-item>
                      <ion-label>Category (left shelf)</ion-label>
                      <ion-select placeholder="Select One" @ionChange="catChange($event, Index,0, trip.shelfSetups[Index].categoriesOnShelf[0].categoryId)" v-model="trip.shelfSetups[Index].categoriesOnShelf[0].categoryId"> <!-- -->
                        <ion-select-option v-for="category in categories" :value="category.id" v-bind:key="category.id">{{ category.name }}</ion-select-option>
                      </ion-select>
                      </ion-item>
                    </ion-col>
                    <ion-col>
                      <ion-item>
                      <ion-label>Category (right shelf)</ion-label>
                      <ion-select placeholder="Select One"  @ionChange="catChange($event, Index,1)" v-model="trip.shelfSetups[Index].categoriesOnShelf[1].categoryId"> <!-- -->
                        <ion-select-option v-for="category in categories" :value="category.id" v-bind:key="category.id">{{ category.name }}</ion-select-option>
                      </ion-select>
                      </ion-item>
                    </ion-col>
                  </ion-row>
                  <ion-row>
                    <ion-col>
                      <ion-item>
                      <ion-label>Category (left hook)</ion-label>
                      <ion-select placeholder="Select One" @ionChange="catChange($event, Index,2)" v-model="trip.shelfSetups[Index].categoriesOnShelf[2].categoryId"> <!-- -->
                        <ion-select-option v-for="category in categories" :value="category.id" v-bind:key="category.id">{{ category.name }}</ion-select-option>
                      </ion-select>
                      </ion-item>
                    </ion-col>
                    <ion-col>
                      <ion-item>
                      <ion-label>Category (right hook)</ion-label>
                      <ion-select placeholder="Select One" @ionChange="catChange($event, Index,3)" v-model="trip.shelfSetups[Index].categoriesOnShelf[3].categoryId"> <!-- -->
                        <ion-select-option v-for="category in categories" :value="category.id" v-bind:key="category.id">{{ category.name }}</ion-select-option>
                      </ion-select>
                      </ion-item>
                    </ion-col>
                  </ion-row>
                </ion-grid>
              </ion-card-content>
            </ion-card>
          </ion-item>
        </ion-list>
        <ion-button expand="block" @click="save()">Save</ion-button>
      </ion-card-content>
    </ion-card>
  </master-layout>
</template>
<script>
import {
  IonCard,
  IonCardContent,
  IonItem,
  IonLabel,
  IonInput
} from "@ionic/vue";
import { mapGetters, mapActions} from "vuex"; 
export default {
  components: {
    IonCard,
    IonCardContent,
    IonItem,
    IonLabel,
    IonInput
  },
  data() {
    return {
      test: 1,
      categories: [],
      shelves: [],
      shelfSetup: {
        shelfId: "",
        categoriesOnShelf: []
      },
      trip: {
        id: "",
        location: "",
        StartDateTime: new Date(),
        EndDateTime: new Date(),
        reminderSeconds: null,
        shelfSetups: [
          {
            shelfId: "",
            categoriesOnShelf: [
              {
                categoryId: parseInt(""),
                index: parseInt("0")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("1")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("2")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("3")
              }
            ]
          },
          {
            shelfId: "",
            categoriesOnShelf: [
              {
                categoryId: parseInt(""),
                index: parseInt("0")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("1")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("2")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("3")
              }
            ]
          },
          {
            shelfId: "",
            categoriesOnShelf: [
              {
                categoryId: parseInt(""),
                index: parseInt("0")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("1")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("2")
              },
              {
                categoryId: parseInt(""),
                index: parseInt("3")
              }
            ]
          }
        ]
      }
    };
  },
  computed: {
    ...mapGetters("auth", {
        authData: "getUser",
    }),
    ...mapGetters("shelves", {
        uploadStatus: "getUploadStatus",
    }),
  },
  async created () {
    await this.fetchData()
  },
  async updated () {
    await this.fetchData()
  },
  methods: {
    ...mapActions("trips", [
      "getTripById",
      "updateTrip",
      "newTrip",
    ]),
    ...mapActions("shelves", [
      "getShelves",
    ]),
    ...mapActions("categories", [
      "getCategories",
    ]),
    async save(){  //prepares date so that it can be send to back-end
      var token = this.authData.token
      for (let i = 0; i < this.trip.shelfSetups.length; i++){
        if (this.shelves[i] != undefined){
          this.trip.shelfSetups[i].shelfId = this.shelves[i].id;
        }
      }
      if (this.trip.location && this.trip.StartDateTime && this.trip.EndDateTime && this.trip.reminderSeconds){
        console.log(this.trip)
        var payload = JSON.parse(JSON.stringify(this.trip));
        this.filterEmpty(payload)
        payload.reminderSeconds = parseInt(payload.reminderSeconds)
        var success = false;
        var id = payload.id
        if(id == 0){
          delete payload.id
          success = (await this.newTrip({token, payload}))
        }
        if(id > 0){
          success = (await this.updateTrip({token, id, payload}))
        }
        console.log(success)
        if(success){
          this.$router.push("/");
        }else{
          alert('failed to add trip')
        }
      }
      else{alert('Please fill in at least, location, start time, end time and reminder in seconds')}
    },
    async fetchData () { //fetch categories, JWT tokens etc.
      var token = this.authData.token;    
      this.shelves = (await this.getShelves(token));
      if(this.shelves.length == 0){
        alert("Shelves are not added. Please add at least one shelf to proceed.");
        this.$router.push("/AddEditShelf/0");
      }
      this.categories = (await this.getCategories(token));
      console.log("cat:", this.categories)
      var path = window.location.pathname;
      var segments = path.split("/");
      var id = segments[2];
      if(id != undefined && id > 0){
        var fetchTrip = (await this.getTripById({ token, id}));
        console.log("fetchTrip", fetchTrip)
        this.prepareData(fetchTrip);
      }
    },
    async onChangeStart(val){ //assign value when a user changes it
      this.trip.StartDateTime = val.detail.value
      console.log(this.trip.StartDateTime);
    },
    async onChangeEnd(val){ //assign value when a user changes it
      this.trip.EndDateTime = val.detail.value
      console.log(this.trip.EndDateTime);
    },
    catChange(val, Index, catIndex, test) { //assign value when a user changes it
      console.log(this.trip, test)
      this.trip.shelfSetups[Index].categoriesOnShelf[catIndex].categoryId = val.detail.value;
    },
    numbersOnly(evt) {
      evt = (evt) ? evt : window.event;
      var charCode = (evt.which) ? evt.which : evt.keyCode;
      if ((charCode > 31 && (charCode < 48 || charCode > 57)) && charCode !== 46) {
        evt.preventDefault();
      } else {
        return true;
      }
    },
    prepareData(fetchTrip){ //converts the received data from database to make it easier to work with
      this.trip.id = fetchTrip.id;
      this.trip.location= fetchTrip.location;
      this.trip.StartDateTime = fetchTrip.startDateTime;
      this.trip.EndDateTime = fetchTrip.endDateTime;
      this.trip.reminderSeconds = fetchTrip.reminderSeconds;
      for (let i = 0; i < this.shelves.length; i++){
        for (let j = 0; j < fetchTrip.shelfSetups.length; j++){
          if (this.shelves[i].id == fetchTrip.shelfSetups[j].shelfId){
            this.trip.shelfSetups[i].shelfId = fetchTrip.shelfSetups[j].shelfId
              for(let k = 0; k < fetchTrip.shelfSetups[j].categoriesOnShelf.length; k++){
                var index = fetchTrip.shelfSetups[j].categoriesOnShelf[k].index
                this.trip.shelfSetups[j].categoriesOnShelf[index].categoryId = fetchTrip.shelfSetups[j].categoriesOnShelf[k].categoryId;
              }
            break;
          }
        }
      }
      console.log("after proc", this.trip)
    },
    filterEmpty(payload){ //make sure we only send it if a certain shelf or categorie is not empty
      payload.shelfSetups.splice(this.shelves.length, payload.shelfSetups.length-this.shelves.length); //only send if there is a shelf binded to it
      for (let i = 0; i < payload.shelfSetups.length; i++){
        for (let j = 0; j < payload.shelfSetups[i].categoriesOnShelf.length; j++){
          if (!this.isNumeric(payload.shelfSetups[i].categoriesOnShelf[j].categoryId)){
            payload.shelfSetups[i].categoriesOnShelf.splice(j,1)
            j--
          }
        }
      }
    },
    isNumeric(n) {
      return !isNaN(parseFloat(n)) && isFinite(n);
    }
  }
}

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