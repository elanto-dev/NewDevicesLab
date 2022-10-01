using System;
using System.Collections.Generic;
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
    //[Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
    public class ShelvesController : ControllerBase
    {
        private readonly ShelfRepository _shelfRepository;

        public ShelvesController(AppDbContext context)
        {
            _shelfRepository = new ShelfRepository(context);
        }

        // GET: api/Shelves
        [HttpGet]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<IEnumerable<Shelf>>> GetShelves()
        {
            return await _shelfRepository.GetByUserId(User.GetUserId());
        }

        // GET: api/Shelves/5
        [HttpGet("id/{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<Shelf>> GetShelf(int id)
        {
            var shelf = await _shelfRepository.GetById(id, User.GetUserId());

            if (shelf == null)
            {
                return NotFound();
            }

            return shelf;
        }

        // GET: api/Shelves/AX52
        [HttpGet("{identifier}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<Shelf>> GetShelfByIdentifier(string identifier)
        {
            var shelf = await _shelfRepository.GetByIdentifier(identifier, User.GetUserId());

            if (shelf == null)
            {
                return NotFound();
            }

            return shelf;
        }

        // PUT: api/Shelves/5
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [HttpPut("{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<IActionResult> PutShelf(int id, Shelf shelf)
        {
            if (id != shelf.Id)
            {
                return BadRequest();
            }

            shelf.UserId = User.GetUserId();
            try
            {
                _shelfRepository.Update(shelf);
                await _shelfRepository.SaveChangesAsync();
            }
            catch (Exception e)
            {
                if (!(await _shelfRepository.Exists(id)))
                {
                    return NotFound();
                }
                else
                {
                    throw e;
                }
            }
            return Ok();
        }

        // POST: api/Shelves
        // To protect from overposting attacks, enable the specific properties you want to bind to, for
        // more details, see https://go.microsoft.com/fwlink/?linkid=2123754.
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        [HttpPost]
        public async Task<ActionResult<Shelf>> PostShelf(ShelfDto shelfDto)
        {
            var shelf = new Shelf
            {
                Identifier = shelfDto.Identifier,
                Location = shelfDto.Location,
                Main = shelfDto.Main,
                MainShelfId = shelfDto.MainShelfId,
                UserId = User.GetUserId()
            };
            await _shelfRepository.AddAsync(shelf);
            await _shelfRepository.SaveChangesAsync();

            return CreatedAtAction("GetShelf", new { id = shelf.Id }, shelf);
        }

        // DELETE: api/Shelves/5
        [HttpDelete("{id}")]
        [Authorize(AuthenticationSchemes = JwtBearerDefaults.AuthenticationScheme)]
        public async Task<ActionResult<Shelf>> DeleteShelf(int id)
        {
            var shelf = await _shelfRepository.GetById(id, User.GetUserId());
            if (shelf == null)
            {
                return NotFound();
            }

            _shelfRepository.Delete(shelf);
            await _shelfRepository.SaveChangesAsync();

            return shelf;
        }
    }
}
