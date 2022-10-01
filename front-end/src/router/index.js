import { createRouter, createWebHistory } from "@ionic/vue-router";
import Login from "../pages/Login.vue";
import Register from "../pages/Register.vue";
import Main from "../pages/Main.vue";
import Trips from "../pages/AddEditTrip.vue";
import AddEditShelf from "../pages/AddEditShelf.vue";
import AddEditTrip from "../pages/AddEditTrip.vue";
import store from "../store/index";

const routes = [
  {
    path: "/",
    redirect: "/login",
  },
  {
    path: "/login",
    component: Login,
  },
  {
    path: "/register",
    component: Register,
  },
  {
    path: "/Main",
    component: Main,
    meta: { requiredAuth: true },
  },
  {
    path: "/Trips",
    component: Trips,
    meta: { requiredAuth: true },
  },
  {
    path: "/AddEditShelf/:id",
    component: AddEditShelf,
    props: true,
    meta: { requiredAuth: true },
  },
  {
    path: "/AddEditTrip/:id",
    component: AddEditTrip,
    props: true,
    meta: { requiredAuth: true },
  },
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
});

function guard(to, from, next, token) {
  if (to.meta && to.meta.requiredAuth) {
    if (token != "") {
      return next();
    }
    return next({ path: "/login" });
  } else {
    if (token != "") {
      return next({ path: "/Main" });
    }
    return next();
  }
}

router.beforeEach((to, from, next) => {
  let token = store.getters["auth/getAuthData"];
  if (token.userId == 0) {
    store.dispatch("auth/loadStorageTokens").then(
      () => {
        token = store.getters["auth/getAuthData"];
        return guard(to, from, next, token);
      },
      (error) => {
        console.log(error);
        return guard(to, from, next, token);
      }
    );
  } else {
    return guard(to, from, next, token);
  }
});

export default router;
