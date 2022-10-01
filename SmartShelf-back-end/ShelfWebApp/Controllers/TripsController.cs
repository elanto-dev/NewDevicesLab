using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using DAL;
using DAL.Repositories;
using Domain;
using Identity;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using ShelfWebApp.PublicAPI;

namespace ShelfWebApp.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class TripsController : ControllerBase
    {
        private readonly TripRepository _tripRepository;
        private readonly ShelfInTripRepository _shelfInTripRepository;
        private readonly ShelfSetupCategoriesRepository _shelfSetupCategoriesRepository;
        private const int CategoriesInShelfCount = 4;


        public TripsController(AppDbContext context)
        {
            _tripRepository = new TripRepository(context);
            _shelfInTripRepository = new ShelfInTripRepository(context);
            _shelfSetupCategoriesRepository = new ShelfSetupCategoriesRepository(context);
        }

        // GET: api/Trips
        [HttpGet]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<IEnumerable<TripDto>>> GetTrips()
        {
            var trips = await _tripRepository.GetByUserId(User.GetUserId());
            var tripsDtos = new List<TripDto>();
            foreach (var trip in trips)
            {
                var tripDto = await GetTripDtoFromTrip(trip);
                tripsDtos.Add(tripDto);
            }

            return tripsDtos;
        }

        // POST: api/Trips/Shelf/UserLeft/5
        [HttpPost("Shelf/UserLeft/{tripId}")]
        public async Task<ActionResult<Shelf>> PostUserLeft(int tripId)
        {

            var trip = _tripRepository.GetById(tripId).Result;
            if (trip == null)
            {
                return BadRequest();
            }

            try
            {
                trip.UserLeftTheHouse = true;
                _tripRepository.Update(trip);
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!(await _tripRepository.Exists(tripId)))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/Trips/Shelf/1/AZ421/Glasses/ItemPresent/true
        [HttpPost("Shelf/{tripId}/{identifier}/{category}/ItemPresent/{placed}")]
        public async Task<ActionResult<Shelf>> PostItemPlacementChanged(int tripId, string identifier, string category, bool placed)
        {
            var shelfInTrip = (await _shelfInTripRepository.GetByTripId(tripId)).FirstOrDefault(s => s.Shelf.Identifier == identifier);
            if (shelfInTrip == null)
            {
                return BadRequest();
            }

            var shelfCategory = (await _shelfSetupCategoriesRepository.GetByShelfInTrip(shelfInTrip.Id)).FirstOrDefault(c => c.Category.Name == category);
            if (shelfCategory == null)
            {
                return BadRequest();
            }
            if (shelfCategory.ItemRemoved == placed) return NoContent();

            try
            {
                shelfCategory.ItemRemoved = placed;
                _shelfSetupCategoriesRepository.Update(shelfCategory);
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!(await _tripRepository.Exists(tripId)))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // GET: api/Trips/Shelf/AX52
        [HttpGet("Shelf/{index}")]
        public async Task<ActionResult<TripForShelfSetupDto>> GetTripForShelf(string index)
        {
            var shelfInTrips = await _shelfInTripRepository.GetByShelfIdentifierAsync(index);
            var shelf = shelfInTrips.Where(s => s.Trip.StartDateTime >= DateTime.Now - TimeSpan.FromHours(2) && !s.Trip.UserLeftTheHouse)
                .OrderBy(s => s.Trip.StartDateTime).FirstOrDefault();
            var tripForShelfDto = new TripForShelfSetupDto{ TripId = -1 };
            if (shelf != null)
            {
                tripForShelfDto.TripId = shelf.TripId;
                tripForShelfDto.ReminderDateTime = shelf.Trip.ReminderInAdvanceSeconds == null
                    ? new DateTime()
                    : shelf.Trip.StartDateTime.AddSeconds(-(int) shelf.Trip.ReminderInAdvanceSeconds);
                tripForShelfDto.EarliestCheckOutDateTime = shelf.Trip.StartDateTime.AddHours(-1);
                tripForShelfDto.Categories = new List<TripForShelfSetupDto.CategoriesSetupDto>();
                foreach (var setupCategory in shelf.ShelfSetupCategories)
                {
                    tripForShelfDto.Categories.Add(new TripForShelfSetupDto.CategoriesSetupDto
                    {
                        Index = setupCategory.Index,
                        Name = setupCategory.Category.Name
                    });
                }

                if (tripForShelfDto.Categories.Count < 4)
                {
                    for (var i = 0; i <= CategoriesInShelfCount - tripForShelfDto.Categories.Count; i++)
                    {
                        tripForShelfDto.Categories.Add(new TripForShelfSetupDto.CategoriesSetupDto
                            {Index = -1, Name = ""});
                    }
                }
            }

            return tripForShelfDto;
        }

        // GET: api/Trips/5
        [HttpGet("{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<TripDto>> GetTrip(int id)
        {

            var trip = await _tripRepository.GetById(id);

            if (trip == null)
            {
                return NotFound();
            }

            var tripDto = await GetTripDtoFromTrip(trip);

            return tripDto;
        }

        // PUT: api/Trips/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPut("{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<IActionResult> PutTrip(int id, Trip trip)
        {
            if (id != trip.Id)
            {
                return BadRequest();
            }

            try
            {
                _tripRepository.Update(trip);
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!(await _tripRepository.Exists(id)))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/Trips
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPost]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<Shelf>> PostTrip(TripDto tripDto)
        {
            var trip = new Trip
            {
                StartDateTime = tripDto.StartDateTime,
                EndDateTime = tripDto.EndDateTime,
                Location = tripDto.Location,
                ReminderInAdvanceSeconds = tripDto.ReminderSeconds,
                UserId = User.GetUserId(),
                UserLeftTheHouse = false
            };

            var updatedTrip = await _tripRepository.AddAsync(trip);
            await _tripRepository.SaveChangesAsync();
            tripDto.Id = updatedTrip.Entity.Id;

            foreach (var shelfSetup in tripDto.ShelfSetups)
            {
                var shelfInTripSetup = new ShelfInTrip
                {
                    TripId = updatedTrip.Entity.Id,
                    ShelfId = shelfSetup.ShelfId,
                    ShelfSetupCategories = new List<ShelfSetupCategories>()
                };
                var updatedShelfInTrip = await _shelfInTripRepository.AddAsync(shelfInTripSetup);
                await _shelfInTripRepository.SaveChangesAsync();
                foreach (var categoriesSetupDto in shelfSetup.CategoriesOnShelf)
                {
                    var shelfSetupCategory = new ShelfSetupCategories
                    {
                        CategoryId = categoriesSetupDto.CategoryId,
                        Index = categoriesSetupDto.Index,
                        ShelfInTripId = updatedShelfInTrip.Entity.Id,
                        ItemRemoved = false
                    };
                    await _shelfSetupCategoriesRepository.AddAsync(shelfSetupCategory);
                }

                await _shelfSetupCategoriesRepository.SaveChangesAsync();
            }

            await _shelfInTripRepository.SaveChangesAsync();

            return CreatedAtAction("GetTrip", new { id = tripDto.Id }, tripDto);
        }

        // DELETE: api/Trips/5
        [HttpDelete("{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<Trip>> DeleteTrip(int id)
        {
            var trip = await _tripRepository.GetById(id);
            if (trip == null)
            {
                return NotFound();
            }

            var shelvesInTrip = _shelfInTripRepository.GetByTripId(id).Result;
            if (shelvesInTrip != null && shelvesInTrip.Count > 0)
            {
                foreach (var shelfInTrip in shelvesInTrip)
                {
                    var categoriesInShelf = _shelfSetupCategoriesRepository.GetByShelfInTrip(shelfInTrip.Id).Result;
                    if (categoriesInShelf != null && categoriesInShelf.Count > 0)
                    {
                        foreach (var categoryInShelf in categoriesInShelf)
                        {
                            _shelfSetupCategoriesRepository.Delete(categoryInShelf);
                        }
                        await _shelfSetupCategoriesRepository.SaveChangesAsync();
                    }
                    _shelfInTripRepository.Delete(shelfInTrip);
                }

                await _shelfInTripRepository.SaveChangesAsync();
            }

            _tripRepository.Delete(trip);
            await _tripRepository.SaveChangesAsync();

            return trip;
        }

        private async Task<TripDto> GetTripDtoFromTrip(Trip trip)
        {
            var tripDto = new TripDto
            {
                Id = trip.Id,
                Location = trip.Location,
                StartDateTime = trip.StartDateTime,
                EndDateTime = trip.EndDateTime,
                ReminderSeconds = trip.ReminderInAdvanceSeconds,
                UserId = trip.UserId,
                ShelfSetups = new List<ShelvesSetupDto>()
            };
            var shelvesForTripSetups = await _shelfInTripRepository.GetByTripId(trip.Id);
            foreach (var shelvesForTripSetup in shelvesForTripSetups)
            {
                var shelfSetupDto = new ShelvesSetupDto
                {
                    ShelfId = shelvesForTripSetup.ShelfId,
                    Identifier = shelvesForTripSetup.Shelf.Identifier,
                    CategoriesOnShelf = new List<CategoriesSetupDto>()
                };
                var categories = await _shelfSetupCategoriesRepository.GetByShelfInTrip(shelvesForTripSetup.Id);
                foreach (var category in categories)
                {
                    var categoriesSetupDto = new CategoriesSetupDto
                    {
                        CategoryId = category.CategoryId,
                        Index = category.Index,
                        Name = category.Category.Name,
                        ItemRemoved = category.ItemRemoved
                    };
                    shelfSetupDto.CategoriesOnShelf.Add(categoriesSetupDto);
                }
                tripDto.ShelfSetups.Add(shelfSetupDto);
            }

            return tripDto;
        }
    }
}
