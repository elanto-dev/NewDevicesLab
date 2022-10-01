using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Domain;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace DAL.Repositories
{
    public class ShelfInTripRepository : BaseRepository
    {
        private readonly AppDbContext _context;

        public ShelfInTripRepository(AppDbContext context) : base(context)
        {
            _context = context;
        }

        public async Task<List<ShelfInTrip>> GetByTripId(int tripId)
        {
            return await _context.ShelfInTrips.Where(t => t.TripId == tripId).Include(s => s.Shelf).AsNoTracking().ToListAsync();
        }

        public async Task<List<ShelfInTrip>> GetByShelfIdentifierAsync(string identifier)
        {
            return await _context.ShelfInTrips
                .Include(s => s.Shelf)
                .Include(t => t.Trip)
                .Include(s => s.ShelfSetupCategories)
                .ThenInclude(c => c.Category)
                .AsNoTracking()
                .Where(s => s.Shelf.Identifier == identifier)
                .ToListAsync();
        }

        public EntityEntry<ShelfInTrip> Update(ShelfInTrip shelfInTrip)
        {
            return _context.ShelfInTrips.Update(shelfInTrip);
        }

        public async Task<EntityEntry<ShelfInTrip>> AddAsync(ShelfInTrip shelfInTrip)
        {
            return await _context.ShelfInTrips.AddAsync(shelfInTrip);
        }

        public EntityEntry<ShelfInTrip> Delete(ShelfInTrip shelfInTrip)
        {
            return _context.ShelfInTrips.Remove(shelfInTrip);
        }
    }
}
